#include <PowerFeather.h>

using namespace PowerFeather; // for PowerFeather::Board

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  delay(1000);

  printf("===============================\n");
  printf("PowerFeather - Enter Ship Mode Example\n");
  printf("===============================\n");
  printf("\n\n");

  if (Board.init(500) == Result::Ok) // check if initialization succeeded
  {
    printf("Board initialized successfully\n\n");

    // Ship mode can't be entered into while external supply is present,
    // loop here while that is the case.
    bool supplyGood = false;
    while (Board.checkSupplyGood(supplyGood) == Result::Ok && supplyGood)
    {
        delay(10);
    }

    delay(1000);

    Board.enterShipMode(); // after a 1 s delay, enter ship mode
    // Ship mode can be exited either by:
    //   - plugging in a supply
    //   - pulling down QON for at least 800 ms
  }
}

void loop()
{
  // Do nothing
}

