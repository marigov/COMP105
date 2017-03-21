#include "simpletools.h"
#include "abdrive.h"
#include "ping.h"

#define WHEEL_DIAMETRE 6.62
#define ROBOT_WIDTH 10.58
#define WHEEL_ENCODER_TICKS 64
#define CELL_WIDTH 41.5
#define TICK_LENGTH 0.325


// Number of cells for X,Y in the 4x4 grid
#define GRID_CELLS_X 4
#define GRID_CELLS_Y 4

// Direction
#define N 0
#define E 1
#define S 2
#define W 3

// Sensing positions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Direction of the robot N, E, S, W
typedef struct direction {
    unsigned int value  : 2;
} Direction;

// Direction of the robot N, E, S, W
typedef struct position {
    int x;
    int y;
    Direction direction;
} Position;

// Cell definition

typedef struct cell {
    _Bool border[4];

} Cell;


Position position;

void setInitialPosition() {
    position.x = 0;
    position.y = -1; // Will start from (0,0)
    position.direction.value = N;
}

Cell grid[4][4];

void setInitialGrid() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            grid[i][j].border[N] = 1;
            grid[i][j].border[E] = 1;
            grid[i][j].border[S] = 1;
            grid[i][j].border[W] = 1;
        }
    }
}

_Bool senseUp() {
    return ping_cm(8) > 30;
}

_Bool senseRight() {
    freqout(1, 1, 38000);
    return input(2) != 0;
}

_Bool senseLeft() {
    freqout(11, 1, 38000);
    return input(10) != 0;
}

_Bool senseDown() {
    return 1;
}

typedef _Bool (*SenseFunc)( void );

SenseFunc *sense;

void initSenseFuncs() {
    sense = (SenseFunc *) malloc(sizeof(SenseFunc) * 4);
    sense[UP] = senseUp;
    sense[RIGHT] = senseRight;
    sense[DOWN] = senseDown;
    sense[LEFT] = senseLeft;
}

void getBorders() {
    Direction d;
    for (int i = 0; i < 4; i++) {
        d.value = position.direction.value + i;
        grid[position.x][position.y].border[d.value] = sense[i]();
    }

    /*  print(
          "North: %d, East: %d, South: %d, West: %d\n",
          grid[position.x][position.y].border[N],
          grid[position.x][position.y].border[E],
          grid[position.x][position.y].border[S],
          grid[position.x][position.y].border[W]
      );*/
}

// It will fill the borders of the unvisited cells, by the adjacent cells

void getAllBorders() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j].border[N] && grid[i][j].border[E] && grid[i][j].border[S] && grid[i][j].border[W]) {
                grid[i][j].border[N] = grid[i][j + 1].border[S];
                grid[i][j].border[E] = grid[i + 1][j].border[W];
                grid[i][j].border[S] = grid[i][j - 1].border[N];
                grid[i][j].border[W] = grid[i - 1][j].border[E];
            }
        }
    }
}

void updateDirection(int update) {
    position.direction.value += update;
}

int getAxis() {
    return position.direction.value % 2;
}

void updatePosition() {
    int axis = getAxis();
    if (axis == 1) {
        if (position.direction.value == 1) {
            position.x += 1;
        } else {
            position.x -= 1;
        }
    } else {
        if (position.direction.value == 0) {
            position.y += 1;
        } else {
            position.y -= 1;
        }
    }
}
// Basic movement functionalities and calculations of leftovers

double cmToTicks(double cm) {
    return cm / TICK_LENGTH;
}

double degToRad(double degrees) {
    return degrees * (PI / 180);
}

double normAngle(double degrees) {
    while (degrees > 180) {
        degrees -= 360;
    }
    while (degrees <= -180) {
        degrees += 360;
    }
    return degrees;
}

void zeroRadiusTurn(double degrees) {
    static double leftovers = 0;
    degrees = normAngle(degrees);
    double theta = degToRad(degrees);
    double arc = cmToTicks(theta * ROBOT_WIDTH / 2);
    int ticks = (int) round(arc);
    leftovers += arc - ticks;
    while (leftovers >= 1) {
        leftovers--;
        ticks++;
    }
    while (leftovers <= -1) {
        leftovers++;
        ticks--;
    }
    drive_goto(ticks, -ticks);
}

void goForward() {
    double cm = CELL_WIDTH;
    static double leftovers[2] = { 0, 0 };
    int axis = getAxis();
    double distance = cmToTicks(cm);
    int ticks = (int) round(distance);
    leftovers[axis] += distance - ticks;
    while (leftovers[axis] >= 1) {
        leftovers[axis]--;
        ticks++;
    }
    while (leftovers[axis] <= -1) {
        leftovers[axis]++;
        ticks--;
    }
    drive_goto(ticks, ticks);
    updatePosition();
}

void turnClock() {
    zeroRadiusTurn(90);
    updateDirection(1);
}

void turnAntiClock() {
    zeroRadiusTurn(-90);
    updateDirection(-1);
}

int cells[4][4] =
{
    {1, 5, 9, 13},
    {2, 6, 10, 14},
    {3, 7, 11, 15},
    {4, 8, 12, 16}

};

int flag = 0;
int currentCell;

int fwdPath[30];
int fwdPathLength = 0;
int bckPath[30];
int bckPathLength = 0;


void addToFwdPath() {
    fwdPath[fwdPathLength] = currentCell;
    fwdPathLength++;
}

void addToBcwPath() {
    bckPath[bckPathLength] = currentCell;
    bckPathLength++;
}

void followWall() {
    getBorders();
    grid[position.x][position.y].border[S] = 0; //Since we come from the bottom
    goForward();
    while (!(position.x == 0 && position.y == -1)) {
        getBorders();
        currentCell = cells[position.x][position.y];
        if (currentCell == 16) {
            flag = 1;
        }
        if (flag == 0) {
            addToFwdPath();
        } else {
            addToBcwPath();
        }
        print("Cell: %d\n",  currentCell);
        fflush(stdout);
        Direction right;
        right.value = position.direction.value + RIGHT;
        if (grid[position.x][position.y].border[right.value]) {
            turnClock();
            goForward();
        } else if (grid[position.x][position.y].border[position.direction.value]) {
            goForward();
        } else {
            while (!grid[position.x][position.y].border[position.direction.value]) {
                turnAntiClock();
            }
            goForward();
        }
        //print("X: %d, Y: %d\n", position.x, position.y);
    }
    getAllBorders();
    zeroRadiusTurn(180);
}

void printPath(int *path, int length) {
    for (int i = 0; i < length; i++ ) {
        print("%d ", path[i]);
        fflush(stdout);
    }
    print("\n");
    fflush(stdout);

}

int main() {

    drive_goto(20, 20);
    initSenseFuncs();
    setInitialPosition();
    setInitialGrid();

    followWall();

    printPath(fwdPath, fwdPathLength);
        printPath(bckPath, bckPathLength);



    return 0;
}
