#include "simpletools.h"
#include "abdrive.h"
#include "ping.h"

#define WHEEL_DIAMETRE 6.62
#define ROBOT_WIDTH 10.58
#define WHEEL_ENCODER_TICKS 64
#define CELL_WIDTH 41
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

char cells[5][4] = {
    {'a', 'e', 'i', 'm'},
    {'b', 'f', 'j', 'n'},
    {'c', 'g', 'k', 'o'},
    {'d', 'h', 'l', 'p'},
};



char *finalGo;
char *raceFwd;
char *raceBck;

char *finalBack;

int lenghtFGo;
int lenghtFBack;

int X = 0, Y = 0;
char currentCell;


char pathGo[25];
int orientationGo[25];

char pathBack[25];
int orientationBack[25];

int lengthGo = 0;
int lengthBack = 0;

int addToPathTo()
{
    pathGo[lengthGo] = cells[position.x][position.y];
    orientationGo[lengthGo] = position.direction.value;
    lengthGo++;
    return 1;
}

int addToPathBack()
{
    pathBack[lengthBack] = cells[position.x][position.y];
    orientationBack[lengthBack] = position.direction.value;
    lengthBack++;
    return 1;
}

int flag = 0;

void followWall() {
    getBorders();
    grid[position.x][position.y].border[S] = 0; //Since we come from the bottom
    goForward();
    while (!(position.x == 0 && position.y == -1)) {
        getBorders();
        currentCell = cells[position.x][position.y];
        print("Cell: %c Orientation: %d\n", currentCell, position.direction.value);
        if (flag == 0) {
            addToPathTo();
        } else {
            addToPathBack();
        }
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
        if (currentCell == 'p') {
            flag = 1;
            addToPathBack();
        }
        //print("X: %d, Y: %d\n", position.x, position.y);
    }
    getAllBorders();
    zeroRadiusTurn(180);
}

char* join(char* path, int start, int end, int length) {
    int index = 0;
    char *newPath = malloc(((end - start) + 1) * sizeof(char));
    for (int i = 0; i < length; i++)
    {
        if ((i <= start) || (i > end))
        {
            newPath[index] = path[i];
            index++;
        }
    }
    return newPath;

}

char* removeDuplicates(char* path, int length) {
    for (int i = 0; i < length; i++) {
        for (int j = length - 1; j > i; j--) {
            if (path[j] == path[i]) {
                char *newPath = malloc((length - (j - i)) * sizeof(char));
                newPath = removeDuplicates(join(path, i, j, length), length - (j - i));
                return newPath;
            }
        }
    }
    return path;
}

void printPath(char* path, int lenght) {
    for (int i = 0; i < lenght; i++) {
        print("%c", path[i]);
    }
    print("\n");
}

int getLength(char* path) {
    int i = 0;
    while (path[i] != '\0') {
        i++;
    }
    return i;
}

int* setIndexs(char *path, char *shortPath, int length, int lengthShort)
{
    int *indices = malloc(lengthShort * sizeof(int));
    int index = 0;
    for (int i = 0; i < lengthShort; i++)
    {
        for (int j = i; j < length; j++)
        {
            if (shortPath[i] == path[j])
            {
                indices[index] = j;
                index++;
                j = length;
            }
        }
    }
    return indices;
}

void getRaceFwd() {
    int* indexs  = setIndexs(pathGo, finalGo, lengthGo, lenghtFGo);
    for (int i = 0; i < lenghtFGo; i++) {
        print("Cell: %c Orientation: %d \n", finalGo[i], orientationGo[indexs[i]]);
    }
    print("Straight\n");
    for (int i = 0; i < lenghtFGo - 1; i++) {
        int diff = orientationGo[indexs[i]] - orientationGo[indexs[i + 1]];
        if (diff == 0) {
            print("Straight\n");
            raceFwd[i] = 's';
        } else if (diff == -1) {
            print("Right\n");
            raceFwd[i] = 'r';

        } else if (diff == 1) {
            print("Left\n");
            raceFwd[i] = 'l';
        }
    }
}

void getRaceBck() {
    int* indexs  = setIndexs(pathBack, finalBack, lengthBack, lenghtFBack);
    int* raceBckTMP = malloc(sizeof(int) * lenghtFBack);

    for (int i = 0; i < lenghtFBack; i++) {
        print("Index: %d Cell: %c Orientation: %d \n", i, finalBack[i], orientationBack[indexs[i]]);
        print("Index race: %d \n", (lenghtFBack - 1) - i);
        raceBckTMP[(lenghtFBack) - i] = orientationBack[indexs[i]];
    }

    raceBckTMP[0] = 2;

    print("Straight\n");
    for (int i = 0; i < lenghtFBack; i++) {
        int diff = raceBckTMP[i]-raceBckTMP[i+1];
        print("Diff: %d \n", diff);
        if (diff == 0) {
            print("Straight\n");
            raceBck[i] = 's';
        } else if (diff == -1) {
            print("Right\n");
            raceBck[i] = 'r';

        } else if (diff == 1) {
            print("Left\n");
            raceBck[i] = 'l';
        }
      
    }


}

void driveBck() {
    goForward();
     for (int i = 0; i < lenghtFBack; i++) {
        print("Direction: %c \n", raceBck[i]);
        if (raceBck[i] == 's') {
            goForward();
        } else if (raceBck[i] == 'r') {
            turnClock();
            goForward();

        } else if (raceBck[i] == 'l') {
            turnAntiClock();
            goForward();

        }
    }
}
void driveFwd() {
    goForward();
    for (int i = 0; i < lenghtFGo - 1; i++) {
        print("Direction: %c \n", raceFwd[i]);
        if (raceFwd[i] == 's') {
            goForward();
        } else if (raceFwd[i] == 'r') {
            turnClock();
            goForward();

        } else if (raceFwd[i] == 'l') {
            turnAntiClock();
            goForward();

        }
    }
}

int main() {

    drive_goto(20, 20);
    initSenseFuncs();
    setInitialPosition();
    setInitialGrid();

    followWall();

    finalGo = removeDuplicates(pathGo, lengthGo);
    lenghtFGo = getLength(finalGo) - 1;
    finalBack = removeDuplicates(pathBack, lengthBack);
    lenghtFBack = getLength(finalBack);

    raceFwd = malloc(sizeof(char) * lenghtFGo);
    raceBck = malloc(sizeof(char) * lenghtFBack);


    printPath(pathGo, lengthGo);
    printPath(pathBack, lengthBack);
    printPath(finalGo, lenghtFGo);
    printPath(finalBack, lenghtFBack);
    getRaceFwd();
    print("-- \n");
    getRaceBck();




    if (lenghtFBack < lenghtFGo) {
        print("2nd path");
        driveBck();
    } else {
        print("1st path");
        driveFwd();
    }

    return 0;
}
