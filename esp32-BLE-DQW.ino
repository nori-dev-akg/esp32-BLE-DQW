/**
 * ESP32 BLE Mouse & iOS & DQW
 */
#include <BleMouse.h>

//#define MOUSE_ANALYZE

#ifdef MOUSE_ANALYZE
#define PIN_CLICK GPIO_NUM_23
#define PIN_LEFT GPIO_NUM_22
#define PIN_RIGHT GPIO_NUM_21
#define PIN_UP GPIO_NUM_19
#define PIN_DOWN GPIO_NUM_18
#define PIN_ZERO GPIO_NUM_5
#endif

#define PIN_SEARCH GPIO_NUM_17

#define D_WIDTH 1220
#define D_HEIGHT 1500

#define MAX_COUNT 50    // 連続実行回数

BleMouse bleMouse;

int cx = 0, cy = 0;
int xpos = 0, ypos = 0;
int count = 0;

void setup()
{
    Serial.begin(115200);

#ifdef MOUSE_ANALYZE
    pinMode(PIN_CLICK, INPUT_PULLUP);
    pinMode(PIN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_ZERO, INPUT_PULLUP);
#endif
    pinMode(PIN_SEARCH, INPUT_PULLUP);

    attachInterrupt(PIN_SEARCH, Search, FALLING);

    Serial.println("Starting BLE work!");
    bleMouse.begin();
}

volatile bool bSearch = false;
void SearchForMonster()
{
    int x, y, xs = 0, xe = 1200, ys = 450, ye = 1070;
    for (y = ys; y < ye; y += 40)
    {
        for (x = xs; x < xe && bSearch; x += 40)
        {
            moveTo(x, y);
            delay(20);
            bleMouse.click(MOUSE_LEFT);
            delay(20);
        }
        for (x = xe, y += 40; x > xs && bSearch; x -= 40)
        {
            moveTo(x, y);
            delay(20);
            bleMouse.click(MOUSE_LEFT);
            delay(20);
        }
    }
}
void Search()
{
    bSearch = !bSearch;
}

void loop()
{
    if (bleMouse.isConnected())
    {
        if (bSearch)
        {
            if (count == 0)
            {
                moveToZero();
            }
            SearchForMonster();
            count++;
            if (count > MAX_COUNT)
            {
                bSearch = false;
            }
        }
        else
        {
            count = 0;
        }

#ifdef MOUSE_ANALYZE
        if (!digitalRead(PIN_ZERO))
        {
            Serial.println("PIN_ZERO");
            moveToZero();
            delay(100);
        }
        if (!digitalRead(PIN_CLICK))
        {
            Serial.println("PIN_CLICK");
            bleMouse.click(MOUSE_LEFT);
            delay(100);
        }
        if (!digitalRead(PIN_LEFT))
        {
            bleMouse.move(-10, 0);
            xpos = xpos > 10 ? xpos - 10 : 0;
            Serial.printf("PIN_LEFT xpos:%d, ypos:%d\n", xpos, ypos);
            delay(100);
        }
        if (!digitalRead(PIN_RIGHT))
        {
            bleMouse.move(10, 0);
            xpos += 10;
            xpos = xpos > D_WIDTH ? D_WIDTH : xpos;
            Serial.printf("PIN_RIGHT xpos:%d, ypos:%d\n", xpos, ypos);
            delay(100);
        }
        if (!digitalRead(PIN_UP))
        {
            bleMouse.move(0, -10);
            ypos = ypos > 10 ? ypos - 10 : 0;
            Serial.printf("PIN_UP xpos:%d, ypos:%d\n", xpos, ypos);
            delay(100);
        }
        if (!digitalRead(PIN_DOWN))
        {
            bleMouse.move(0, 10);
            ypos += 10;
            ypos = ypos > D_HEIGHT ? D_HEIGHT : ypos;
            Serial.printf("PIN_DOWN xpos:%d, ypos:%d\n", xpos, ypos);
            delay(100);
        }
#endif
    }
}

void move(int x, int y)
{
    int xx, yy;
    x = x - cx, y = y - cy;

    //Serial.printf("move(x:%d, y:%d)\n", x, y);

    while (x != 0 || y != 0)
    {
        if (x < -10)
        {
            xx = -10;
            x += 10;
        }
        else if (x > 10)
        {
            xx = 10;
            x -= 10;
        }
        else
        {
            xx = x;
            x = 0;
        }
        if (y < -10)
        {
            yy = -10;
            y += 10;
        }
        else if (y > 10)
        {
            yy = 10;
            y -= 10;
        }
        else
        {
            yy = y;
            y = 0;
        }
        //Serial.printf("xx:%d, yy:%d\n", xx, yy);
        bleMouse.move(xx, yy);
        delay(1);
    }
}

void moveToZero()
{
    // set Zero Position
    cx = 0;
    cy = 0;
    move(-1500, -1500);
    cx = 0;
    cy = 0;
    xpos = 0;
    ypos = 0;
}
void moveTo(int x, int y)
{
    move(x, y);
    //delay(200);
    cx = x;
    cy = y;
}
