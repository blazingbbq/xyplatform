#include "main.h"
#include "driverlib/driverlib.h"
#include "api/lcd.h"
#include "api/keypad.h"
#include "api/stepper.h"

void main(void)
{
  //Turn off interrupts during initialization
  __disable_interrupt();

  //Stop watchdog timer unless you plan on using it
  WDT_A_hold(WDT_A_BASE);

  Init_GPIO();  //Sets all pins to output low as a default
  Init_Clock(); //Sets up the necessary system clocks
  Init_LCD();   //Sets up the LaunchPad LCD display

  Init_KEYPAD(); //Sets up the keypad pins

  PMM_unlockLPM5(); //Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

  //All done initializations - turn interrupts back on.
  //__enable_interrupt();

  // Initialize motors
  uint16_t limPins[2] = {GPIO_PIN7, GPIO_PIN6};
  uint8_t limPorts[2] = {GPIO_PORT_P1, GPIO_PORT_P1};
  uint16_t stepperPins[4] = {GPIO_PIN1, GPIO_PIN1, GPIO_PIN0, GPIO_PIN7};
  uint8_t stepperPorts[4] = {GPIO_PORT_P8, GPIO_PORT_P1, GPIO_PORT_P1, GPIO_PORT_P2};

  uint16_t limPins2[2] = {GPIO_PIN0, GPIO_PIN2};
  uint8_t limPorts2[2] = {GPIO_PORT_P5, GPIO_PORT_P5};
  uint16_t stepperPins2[4] = {GPIO_PIN0, GPIO_PIN1, GPIO_PIN5, GPIO_PIN2};
  uint8_t stepperPorts2[4] = {GPIO_PORT_P8, GPIO_PORT_P5, GPIO_PORT_P2, GPIO_PORT_P8};

  stepper motor; // Stepper X
  motor = initStepper(stepperPorts, stepperPins, limPorts, limPins);
  setSpeed(&motor, 1);
  motor.swap = -1;

  stepper motor2; // Stepper Y
  motor2 = initStepper(stepperPorts2, stepperPins2, limPorts2, limPins2);
  setSpeed(&motor2, 1);

  // Calibrate motors
  int maxRangeX = 0;
  int backX = 0;
  int maxRangeY = 0;
  int backY = 0;
  int max = 25000;

  print("PRESS ANY KEY TO CALIBRATE");
  while (!anyKeyDown()) ;
  print("CLBR8");

  setTarget(&motor, -max);
  setTarget(&motor2, -max);
  while (backX != 2 || backY != 2)
  {
    __delay_cycles(1);
    if (drive(&motor) == -1)
    {
      if (backX != 2)
        backX++;
      if (backX == 1)
      {
        setDist(&motor, 0);
        setTarget(&motor, max);
      }
      if (backX == 2)
      {
        maxRangeX = getLocation(motor) / 2;
        setDist(&motor, maxRangeX);
        setTarget(&motor, maxRangeX);
      }
    }

    if (drive(&motor2) == -1)
    {
      if (backY != 2)
        backY++;
      if (backY == 1)
      {
        setDist(&motor2, 0);
        setTarget(&motor2, max);
      }
      if (backY == 2)
      {
        maxRangeY = getLocation(motor2) / 2;
        setDist(&motor2, maxRangeY);
        setTarget(&motor2, maxRangeY);
      }
    }
  }

  // Move back to origin
  setTarget(&motor, 0);
  setTarget(&motor2, 0);
  int xOrigin = 0;
  int yOrigin = 0;
  while (1)
  {
    __delay_cycles(1);
    // Step to origin
    xOrigin = drive(&motor);
    yOrigin = drive(&motor2);
    if (xOrigin == 1 && yOrigin == 1)
      break;
  }

  // Print max range
  char rangeStr[32];
  sprintf(rangeStr, "MAX RANGE [%d %d]", maxRangeX, maxRangeY);
  print(rangeStr);

  while (1)
  {
    //  User mode
    print("ENTER TARGET COORDINATES");

    int coords[10];
    char coord_out[1028];

    unsigned int i;
    for (i = 0; i < 5; i++)
    {
      coords[2 * i] = nextKeypadValue();
      coords[2 * i + 1] = nextKeypadValue();

      sprintf(coord_out, "C%d [%d %d]", i + 1, coords[2 * i], coords[2 * i + 1]);
      print(coord_out);
    }

    sprintf(coord_out, "COORDS [%d %d] [%d %d] [%d %d] [%d %d] [%d %d]",
            coords[0], coords[1], coords[2], coords[3], coords[4],
            coords[5], coords[6], coords[7], coords[8], coords[9]);
    print(coord_out);

    // Wait for user input to start movement
    print("PRESS ANY KEY TO START");
    while (!anyKeyDown()) ;
    print("STARTING");

    // Movement mode
    char str[16];
    int xState, yState, xProgress, yProgress;
    float initialX = 0;
    float initialY = 0;
    i = 0;
    int j = 0;
    setTarget(&motor, coords[0]);
    setTarget(&motor2, coords[1]);
    while (i < 5)
    {
      __delay_cycles(1);
      xState = drive(&motor);
      yState = drive(&motor2);
      if (xState != 0 && yState != 0)
      {
        i++;
        if (i < 5)
        {
          setTarget(&motor, coords[2 * i]);
          setTarget(&motor2, coords[2 * i + 1]);
          initialX = getLocation(motor);
          initialY = getLocation(motor2);
          clear();
        }
      }
      if (j == 100 || j == 0)
      {
        if (anyKeyDown())
        {
          break;
        }
        j = 0;

        // Display progress
        char *strPadding;
        xProgress = (getTarget(motor) - initialX == 0)
                        ? 100
                        : (int) (((getLocation(motor) - initialX) / (getTarget(motor) - initialX)) * 100);
        yProgress = (getTarget(motor2) - initialY == 0)
                        ? 100
                        : (int) (((getLocation(motor2) - initialY) / (getTarget(motor2) - initialY)) * 100);

        if (yProgress < 10)
            strPadding = "  ";
        else if (yProgress < 100)
            strPadding = " ";
        else
            strPadding = "";

        sprintf(str, "%d%s%d", xProgress, strPadding, yProgress);
        print(str);
      }
      j++;
    }

    // Return to origin
    print("MOVING TO ORIGIN");
    setTarget(&motor, 0);
    setTarget(&motor2, 0);
    while (1)
    {
      __delay_cycles(1);
      // Step to origin
      xOrigin = drive(&motor);
      yOrigin = drive(&motor2);
      if (xOrigin && yOrigin)
        break;
    }
  }
}

void Init_GPIO(void)
{
  // Set all GPIO pins to output low to prevent floating input and reduce power consumption
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

  GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
  GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

  //Set LaunchPad switches as inputs - they are active low, meaning '1' until pressed
  GPIO_setAsInputPinWithPullUpResistor(SW1_PORT, SW1_PIN);
  GPIO_setAsInputPinWithPullUpResistor(SW2_PORT, SW2_PIN);

  //Set LED1 and LED2 as outputs
  //GPIO_setAsOutputPin(LED1_PORT, LED1_PIN); //Comment if using UART
  GPIO_setAsOutputPin(LED2_PORT, LED2_PIN);
}

/* Clock System Initialization */
void Init_Clock(void)
{
  /*
     * The MSP430 has a number of different on-chip clocks. You can read about it in
     * the section of the Family User Guide regarding the Clock System ('cs.h' in the
     * driverlib).
     */

  /*
     * On the LaunchPad, there is a 32.768 kHz crystal oscillator used as a
     * Real Time Clock (RTC). It is a quartz crystal connected to a circuit that
     * resonates it. Since the frequency is a power of two, you can use the signal
     * to drive a counter, and you know that the bits represent binary fractions
     * of one second. You can then have the RTC module throw an interrupt based
     * on a 'real time'. E.g., you could have your system sleep until every
     * 100 ms when it wakes up and checks the status of a sensor. Or, you could
     * sample the ADC once per second.
     */
  //Set P4.1 and P4.2 as Primary Module Function Input, XT_LF
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1 + GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

  // Set external clock frequency to 32.768 KHz
  CS_setExternalClockSource(32768);
  // Set ACLK = XT1
  CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
  // Initializes the XT1 crystal oscillator
  CS_turnOnXT1LF(CS_XT1_DRIVE_1);
  // Set SMCLK = DCO with frequency divider of 1
  CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
  // Set MCLK = DCO with frequency divider of 1
  CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
}
