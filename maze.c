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

// 0 if X 1 if Y

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


// Calcualtes the ticks based on the previous error.

void cleanTurn(double degrees) {
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

// Calcualtes the ticks based on the previous error.

void goFwd() {
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
    cleanTurn(90);
    updateDirection(1);
}

void turnAntiClock() {
    cleanTurn(-90);
    updateDirection(-1);
}

// Cells for storing our path

char cells[5][4] = {
    {'a', 'e', 'i', 'm'},
    {'b', 'f', 'j', 'n'},
    {'c', 'g', 'k', 'o'},
    {'d', 'h', 'l', 'p'},
};

char *fwdFinal;
char *bckFinal;

char *fwdRace;
char *bckRace;

int lenghtFGo;
int lenghtFBack;

char currentCell;

typedef struct cellPosition {
    char c;
    int direction;
    struct cellPosition* next;
    struct cellPosition* prev;

} CellPosition;

CellPosition* GetNewNode(char x, int y) {
    CellPosition* newNode
        = (CellPosition*)malloc(sizeof(CellPosition));
    newNode->c = x;
    newNode->direction = y;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

CellPosition *headFwd = NULL;
CellPosition *headBck = NULL;



void printList(CellPosition* head) {
    CellPosition* temp = head;
    while (temp != NULL) {
        print("%c %d ", temp->c, temp->direction);
        temp = temp->next;
    }
    print("\n");
}

void addFwd()
{
    CellPosition* temp = headFwd;
    CellPosition* newNode = GetNewNode(cells[position.x][position.y], position.direction.value);
    if (headFwd == NULL) {
        headFwd = newNode;
        return;
    }
    while (temp->next != NULL) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}


void addBck()
{
    CellPosition* temp = headBck;
    CellPosition* newNode = GetNewNode(cells[position.x][position.y], position.direction.value);
    if (headBck == NULL) {
        headBck = newNode;
        return;
    }
    while (temp->next != NULL) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

int addLast()
{
    CellPosition* temp = headBck;
    CellPosition* newNode = GetNewNode('z', 2);
    if (headBck == NULL) {
        headBck = newNode;
        return 1;
    }
    while (temp->next != NULL) temp = temp->next; // Go To last Node
    temp->next = newNode;
    newNode->prev = temp;
    return 1;
}

void deleteDuplicates(CellPosition* head) {
    CellPosition* temp = head;
    CellPosition* initialLast;
    CellPosition* tmpLast;

    while (temp->next != NULL) {
        temp = temp->next;
    }

    initialLast = temp;
    tmpLast = initialLast;
    temp = head;

    while (temp != NULL) {
        tmpLast = initialLast;
        //print("Head cell: %c\n", temp->c);
        if (temp->next == NULL) {
            break;
        }
        while (tmpLast != temp) {
            //print("L cell: %c\n", tmpLast->c);
            if (tmpLast->c == temp->c) {
                temp->next = tmpLast->next;
            }
            tmpLast = tmpLast->prev;
        }
        //print("--\n");
        temp = temp->next;
    }
    printList(head);
}

int flag = 0;

void followWall() {
    getBorders();
    grid[position.x][position.y].border[S] = 0; //Since we come from the bottom
    goFwd();
    while (!(position.x == 0 && position.y == -1)) {
        getBorders();
        currentCell = cells[position.x][position.y];
        print("Cell: %c Orientation: %d\n", currentCell, position.direction.value);
        if (flag == 0) {
            addFwd();
        } else {
            addBck();
        }
        Direction right;
        right.value = position.direction.value + RIGHT;
        if (grid[position.x][position.y].border[right.value]) {
            turnClock();
        }
        else if (!grid[position.x][position.y].border[position.direction.value]) {
            while (!grid[position.x][position.y].border[position.direction.value]) {
                turnAntiClock();
            }
        }
        if (currentCell == 'p') {
            flag = 1;
            print("\n");
        }
        goFwd();
        //print("X: %d, Y: %d\n", position.x, position.y);
    }
    addLast();
    getAllBorders();
    cleanTurn(180);
}

int getSize(CellPosition* head) {
    CellPosition* temp = head;
    int i = 0;
    while (temp != NULL) {
        i++;
        temp = temp->next;
    }
    return i;
}

char *commandsFwd;
char *commandsBck;



void generateRaceFwd(CellPosition* head) {
    CellPosition* temp = head;
    int i = 0;
    while (temp->next != NULL) {
        //print("Direction: %d\n", temp->direction - (temp->next)->direction);
        int diff = temp->direction - (temp->next)->direction;
        if (diff == 0) {
            commandsFwd[i] = 's';
            print("Straight\n");
        } else if (diff == -1) {
            commandsFwd[i] = 'r';
            print("Right\n");

        } else if (diff == 1) {
            commandsFwd[i] = 'l';
            print("Left\n");
        }
        else if (diff == -3) {
            commandsFwd[i] = 'l';
            print("Left\n");
        }
        else if (diff == 3) {
            commandsFwd[i] = 'r';
            print("Right\n");
        }
        temp = temp->next;
        if (temp->next == NULL) {
            break;
        }
        i++;
    }
}

void generateRaceBck(CellPosition* head) {
    CellPosition* temp = head;
    int i = 0;
    while (temp->next != NULL) {
        print("Direction: %d\n", temp->direction - (temp->next)->direction);
        int diff = temp->direction - (temp->next)->direction;
        if (diff == 0) {
            commandsBck[i] = 's';
            print("Straight\n");
        } else if (diff == -1) {
            commandsBck[i] = 'r';
            print("Right\n");

        } else if (diff == 1) {
            commandsBck[i] = 'l';
            print("Left\n");
        }
        else if (diff == -3) {
            commandsBck[i] = 'l';
            print("Left\n");
        }
        else if (diff == 3) {
            commandsBck[i] = 'r';
            print("Right\n");
        }
        temp = temp->next;
        if (temp->next == NULL) {
            break;
        }
        i++;
    }
}
int distanceLeft, distanceRight, previousLeft, previousRight;
int values[4][4] = {
    {100, 100, 50, 50},
    {38, -38, 42, -4},
    {-38, 38, -4, 42},
};
void drive(char *arr, int size) {
    goFwd();
    int flag = 0;
    int d = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] == 's') {
            //oFwd();
            d = 0;
        } else if (arr[i] == 'r') {
            // turnClock();
            // goFwd();
            d = 1;
            flag = 1;
        } else if (arr[i] == 'l') {
            // turnAntiClock();
            // goFwd();
            d = 2;
            flag = 1;

        }
        pause(5000);
        distanceLeft = 0;
        distanceRight = 0;
        drive_getTicks(&previousLeft, &previousRight);
        while (distanceLeft <= values[d][0] || distanceRight <= values[d][1])
        {
            drive_speed(values[d][2], values[d][3]);
            drive_getTicks(&distanceLeft, &distanceRight);
            distanceLeft -= previousLeft;
            distanceRight -= previousRight;
        }
        drive_speed(0, 0);
        pause(5000);

        if (flag == 1) {
            flag = 0;
            d = 0;
            distanceLeft = 0;
            distanceRight = 0;
            drive_getTicks(&previousLeft, &previousRight);
            while (distanceLeft <= values[d][0] || distanceRight <= values[d][1])
            {
                drive_speed(values[d][2], values[d][3]);
                drive_getTicks(&distanceLeft, &distanceRight);
                distanceLeft -= previousLeft;
                distanceRight -= previousRight;
            }
        }
        drive_speed(0, 0);

    }
}

void flip(CellPosition** head_ref)
{
    CellPosition* prev   = NULL;
    CellPosition* current = *head_ref;
    CellPosition* next;
    while (current != NULL)
    {
        next  = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    *head_ref = prev;
}

int main() {

    drive_goto(20, 20);
    initSenseFuncs();
    setInitialPosition();
    setInitialGrid();

    headFwd = NULL;
    headBck = NULL;

    followWall();

    deleteDuplicates(headFwd);
    deleteDuplicates(headBck);
    flip(&headBck);

    int sizeFwd = getSize(headFwd);
    int sizeBcw = getSize(headBck);
    commandsFwd = malloc(sizeof(char) * (sizeFwd) - 1);
    commandsBck = malloc(sizeof(char) * (sizeBcw) - 1);

    generateRaceFwd(headFwd);
    generateRaceBck(headBck);

    for (int i = 0; i < sizeFwd - 1; i++) {
        print("%c\n", commandsFwd[i]);
    }
    print("--\n");
    for (int i = 0; i < sizeBcw - 1; i++) {
        print("%c\n", commandsBck[i]);
    }

    if (sizeFwd > sizeBcw) {
        drive(commandsBck, sizeBcw - 1);
    } else {
        drive(commandsFwd, sizeFwd - 1);
    }



    return 0;
}
