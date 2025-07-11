#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

#include "PinMapping.h"
#include "StepperMotor.h"

#define DEBUG_PRINT false
#define baud_rate 9600

enum class args_t
{
  CHANNEL,
  VOLUME,
  TIME,
  RATE,
  ALL,
  CHEM_WASH,
  COMMON,
  GFET,
  COLLECTION,
  NONE
};

const char *arg_names[] = {
    "CHANNEL",
    "VOLUME",
    "TIME",
    "RATE",
    "ALL",
    "CHEM_WASH",
    "COMMON",
    "GFET",
    "COLLECTION",
    "NONE"};

enum class command_types_t
{
  WASH,
  PURGE,
  PRIME,
  PUMP,
  COLLECT,
  WAIT,
  DEBUG,
  NONE
};

const char *command_names[] = {
    "WASH",
    "PURGE",
    "PRIME",
    "PUMP",
    "COLLECT",
    "WAIT",
    "DEBUG",
    "NONE"};

struct command_t
{
  command_types_t command_type;

  args_t arg1;
  int var1;

  args_t arg2;
  int var2;

  args_t arg3;
  int var3;
};

struct debug_values_t
{
  volatile command_t current_command;
  volatile time_t start_time;
  volatile int interval;
  volatile bool enabled;
};

IntervalTimer debug_timer;
debug_values_t debug_values;

AD5245 AD(0x2C);
StepperMotor stepper_motor(&AD, ENA, ON_OFF, HIGH_LOW, DIR);
Adafruit_NeoPixel neo_pixel(1, NEO_PIXEL, NEO_GRB + NEO_KHZ800);

/**
 * Wash command for valves
 *
 * @param arg1 Selection (ALL, PURGE, COMMON, GFET, C)
 * @param var1 If arg1=C, Channel selction (1-8)
 * @param var2 Time (s)
 * @param var3 Rate (uL/s)
 */

// TODO: Implement default wash values (NO CUSTOM)

bool wash(args_t arg1, uint16_t var1, uint16_t var2, uint16_t var3)
{
  resetSystem();

  Serial.printf("Prime %i \n", arg1);

  switch (arg1)
  {
  case args_t::ALL:
    wash(args_t::CHEM_WASH, var1, var2, var3);
    wash(args_t::COMMON, var1, var2, var3);
    wash(args_t::GFET, var1, var2, var3);
    wash(args_t::COLLECTION, var1, var2, var3);
    // airPurge(args_t::ALL, var2, var3);
    break;

  case args_t::CHEM_WASH:
    digitalWrite(PI_SSR, HIGH);
    for (int i = 1; i < 9; i++)
    {
      wash(args_t::CHANNEL, i, var2, var3);
    }
    resetSystem();
    return true;

  case args_t::COLLECTION:
    digitalWrite(WASH_SSR, HIGH);
    break;

  case args_t::COMMON:
    digitalWrite(PI_SSR, HIGH);
    digitalWrite(WASH_SSR, HIGH);
    break;

  case args_t::GFET:
    return false;

    digitalWrite(GI_SSR, HIGH);
    digitalWrite(PO_SSR, HIGH);
    stepper_motor.direction(1);
    break;

  case args_t::CHANNEL:
    if (var1 > 8 || var1 < 1)
    {
      resetSystem();
      return false;
    }
    digitalWrite(CHEMICAL_SSR_ARRAY[var1 -1][1], LOW);
    digitalWrite(CHEMICAL_SSR_ARRAY[var1 -1][0], HIGH);
    digitalWrite(PI_SSR, HIGH);
    break;

  default:
    return false;
  }

  stepper_motor.speed(var3);
  stepper_motor.on(1);
  delay(var2 * 1000);

  resetSystem();
  return true;
}

/**
 * Air purge command for valves
 *
 * @param arg1 Selection (ALL, COMMON)
 * @param var2 Time (s)
 * @param var3 Rate (uL/s)
 */
bool airPurge(args_t arg1, uint16_t var2, uint16_t var3)
{
  resetSystem();

  switch (arg1)
  {
  case args_t::COMMON:
    // Open pump output to waste to not flush the gfet and only the common line.
    digitalWrite(PI_SSR, HIGH);
    digitalWrite(VENT_SSR, HIGH);
    break;

  case args_t::ALL:
    // Should just open vent_ssr and then pump to get all fluid to the end waste
    digitalWrite(VENT_SSR, HIGH);
    break;

  default:
    return false;
  }

  stepper_motor.speed(var3);
  stepper_motor.on(1);
  delay(var2 * 1000);

  resetSystem();
  return true;
}

/**
 * Prime command for valves
 *
 * @param arg1 Selection (ALL, C, CHEM_WASH)
 * @param var1 If arg1 == C -> 1-8. Otherwise unused.
 * @param var2 Time (s)
 * @param var3 Rate (uL/s)
 */
bool prime(args_t arg1, uint16_t var1, uint16_t var2, uint16_t var3)
{
  resetSystem();

  digitalWrite(PI_SSR, HIGH);

  // Serial.printf("Prime %i \n", var1);

  switch (arg1)
  {
  case args_t::ALL:
    // This function is not implmented do to the CHANNEL and CHEM_WASH "10" options.
    // This allows for finer control of the speed and time for the wash v. chems.
    return false;
    break;
    

  case args_t::CHANNEL:
    if (var1 == 10) {
      for (int i = 1; i < 9; i++)
      {
        prime(args_t::CHANNEL, i, var2, var3);
      }
    }

    digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][0], HIGH);
    digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][1], HIGH);

    stepper_motor.speed(0, var3);
    delay(100);
    stepper_motor.on(1);

    wait(var2 * 1000);

    stepper_motor.on(0);
    delay(400);
    break;

  case args_t::CHEM_WASH:
    // Prime wash to selected chem SSR
    stepper_motor.speed(var3);
    delay(100);

    if (var1 == 0)
    {
      digitalWrite(WASH_SSR, HIGH);
      stepper_motor.on(1);
      delay(var2 * 1000);
      stepper_motor.on(0);
      delay(400);
      digitalWrite(WASH_SSR, LOW);
    }
    else if (var1 == 10)
    {
      for (int i = 8; i > 0; i--)
      {
        prime(args_t::CHEM_WASH, i, var2, var3);
      }
    }
    else
    {
      if (var1 > 8 || var1 < 1) return false;
      digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][0], HIGH);
      stepper_motor.on(1);
      delay(var2 * 1000);
      stepper_motor.on(0);
      delay(400);
      digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][0], LOW);
    }
    break;

  default:
    return false;
  }

  resetSystem();
  return true;
}

/**
 * Pump command for valves
 *
 * @param var1 Channel selection (1-8)
 * @param arg2 Volume or Time selection (V, T)
 * @param var2 if arg2 == V -> (uL). if arg2 == T -> (s)
 * @param var3 Rate (uL/s)
 */
bool pump(uint16_t var1, args_t arg2, uint16_t var2, uint16_t var3)
{
  resetSystem();

  if (var1 > 8 || var1 < 1) return false;

  digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][0], HIGH);
  digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][1], HIGH);

  stepper_motor.speed(var3);
  stepper_motor.on(1);

  switch (arg2)
  {
  case args_t::VOLUME:
    // Not implmented yet do to no encoder on pump.
    // delay((var2 / var3) * 1000);
    return false;
    break;

  case args_t::TIME:
    delay(var2 * 1000);
    break;

  default:
    return false;
  }

  resetSystem();
  return true;
}

/**
 * Collect command for valves
 *
 * @param var1 Channel selection (0-9)
 * @param arg2 Volume or Time selection (V, T)
 * @param var2 if arg1 == V -> (uL). if arg1 == T -> (s)
 * @param var3 Rate (uL/s)
 */
bool collect(uint16_t var1, args_t arg2, uint16_t var2, uint16_t var3)
{
  resetSystem();
  // TODO: Implement which valves are open
  // TODO: Add in which channel / vent / wash to flush out collections

  // Open collection and run pump.

  printf("%i", var1);

  if (var1 > 9 | var1 < 0)
  {
    return false;
  }

  if (var1 == 0)
  {
    digitalWrite(WASH_SSR, HIGH);
  }
  else if (var1 == 9)
  {
    digitalWrite(VENT_SSR, HIGH);
  }
  else
  {
    digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][1], HIGH);
    digitalWrite(CHEMICAL_SSR_ARRAY[var1 - 1][0], HIGH);
  }

  digitalWrite(COL_SSR, HIGH);

  stepper_motor.speed(0, var3);
  stepper_motor.on(1);

  switch (arg2)
  {
  case args_t::VOLUME:
    // Currently not implemented as stepper motor has no encoder.
    break;

  case args_t::TIME:
    delay(var2 * 1000);
    break;

  default:
    resetSystem();
    return false;
  }

  stepper_motor.on(0);

  resetSystem();
  return true;
}

/**
 * Wait command for valves
 *
 * @param var1 Time (ms)
 */
bool wait(uint16_t var1)
{
  Serial.flush(); // TODO: Determine if this is needed for ACK to send fast.
  delay(var1);
  return true;
}

/**
 * Wait command for valves
 *
 * @param var1 Time (ms). if 0 disable debug.
 */
bool debug(uint16_t var1)
{
  Serial.flush();
  debug_timer.end();
  delay(debug_values.interval + 100);

  debug_values.enabled = false;
  debug_values.interval = 0;

  if (var1 == 0)
  {
    return true;
  }

  debug_values.interval = max(200, var1);
  debug_values.enabled = true;
  debug_timer.begin(sendDebug, debug_values.interval * 1000);

  return true;
}

/**
 * Read serial
 *
 * @param[out] command_chars The char array that the command is stored too
 */
void readSerial(char *command_chars)
{
  bool end_communication = false;
  uint16_t count = 0;

  time_t t = now();

  while (!end_communication && count < 64)
  {
    if (t - now() > 3000)
    {
      if (DEBUG_PRINT)
        printf("TIMEOUT\n");

      break;
    }
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      if (c == '\n' || c == '\0')
      {
        end_communication = true;
        break;
      }

      if (DEBUG_PRINT)
        printf("\'%c\', ", c);

      command_chars[count++] = c;
    }
  }

  command_chars[count++] = ' ';
  command_chars[count] = '0';

  if (DEBUG_PRINT)
    printf("\nCommand Chars: %s\n", command_chars);
}

/**
 * Helper for parseSerialCommand()
 * Select the command type for parsing
 *
 * @param[out] command_type The command type pointer to be modifying
 * @param[in] str The string to parse
 * @return Returns the command type just set
 */
command_types_t selectCommandType(command_types_t *command_type, String str)
{
  if (DEBUG_PRINT)
    printf("Selecting Commands: %s\n", str.c_str());

  if (str == "WASH")
    *command_type = command_types_t::WASH;
  else if (str == "PURGE")
    *command_type = command_types_t::PURGE;
  else if (str == "PRIME")
    *command_type = command_types_t::PRIME;
  else if (str == "PUMP")
    *command_type = command_types_t::PUMP;
  else if (str == "COLLECT")
    *command_type = command_types_t::COLLECT;
  else if (str == "WAIT")
    *command_type = command_types_t::WAIT;
  else if (str == "DEBUG")
    *command_type = command_types_t::DEBUG;
  else
    *command_type = command_types_t::NONE;

  return *command_type;
}

/**
 * Helper for parseSerialCommand()
 * Select the command type for parsing
 *
 * @param[out] argument The argument type pointer to be modifying
 * @param[in] str The string to parse
 * @return Returns the argument just set
 */
args_t selectArgumentType(args_t *argument, String str)
{
  if (DEBUG_PRINT)
    printf("Selecting Arguments: %s\n", str.c_str());

  if (str == "-C")
    *argument = args_t::CHANNEL;
  else if (str == "-V")
    *argument = args_t::VOLUME;
  else if (str == "-R")
    *argument = args_t::RATE;
  else if (str == "-T")
    *argument = args_t::TIME;
  else if (str == "ALL")
    *argument = args_t::ALL;
  else if (str == "CHEM_WASH")
    *argument = args_t::CHEM_WASH;
  else if (str == "COMMON")
    *argument = args_t::COMMON;
  else if (str == "GFET")
    *argument = args_t::GFET;
  else if (str == "COLLECTION")
    *argument = args_t::COLLECTION;
  else
    *argument = args_t::NONE;

  return *argument;
}

/**
 * Parse the serial data stream into a command
 *
 * @param command_chars The array of chars from the serial stream
 * @return Returns a command struct
 */
command_t parseSerialCommand(char *command_chars)
{
  if (DEBUG_PRINT)
    printf("Starting Parsing\n");

  String command_strings[7];
  uint8_t count = 0;

  char *pch;
  pch = strtok(command_chars, " ");

  while (pch != NULL && count < 7)
  {
    if (DEBUG_PRINT)
      printf("%s ", pch);

    command_strings[count++] = String(pch).trim();

    pch = strtok(NULL, " ");
  }

  if (DEBUG_PRINT)
    printf("\n");

  command_t command;
  selectCommandType(&command.command_type, command_strings[0]);

  selectArgumentType(&command.arg1, command_strings[1]);
  command.var1 = command_strings[2].toInt();

  selectArgumentType(&command.arg2, command_strings[3]);
  command.var2 = command_strings[4].toInt();

  selectArgumentType(&command.arg3, command_strings[5]);
  command.var3 = command_strings[6].toInt();

  if (DEBUG_PRINT)
  {
    printf("Command type: %s\n", command_names[(int)command.command_type]);
    printf("Var 1, 2, 3: %i, %i, %i\n", command.var1, command.var2, command.var3);
  }

  return command;
}

/**
 * Runs the command
 *
 * @param command The inputted command
 */
bool runCommand(command_t command)
{
  if (DEBUG_PRINT)
    printf("Starting Running\n");

  digitalWrite(LED_STATUS, HIGH);

  switch (command.command_type)
  {
  case command_types_t::WASH:
    return wash(command.arg1, command.var1, command.var2, command.var3);

  case command_types_t::PURGE:
    return airPurge(command.arg1, command.var2, command.var3);

  case command_types_t::PRIME:
    return prime(command.arg1, command.var1, command.var2, command.var3);

  case command_types_t::PUMP:
    return pump(command.var1, command.arg2, command.var2, command.var3);

  case command_types_t::COLLECT:
    // COLLECT -C 1 -T 5 -R 10
    return collect(command.var1, command.arg2, command.var2, command.var3);

  case command_types_t::WAIT:
    return wait(command.var1);

  case command_types_t::DEBUG:
    return debug(command.var1);

  default:
    if (DEBUG_PRINT)
      printf("ERROR, UNKOWN COMMAND TYPE!\n\n");

    break;
  }

  return true;
}

/**
 * Checks if the command type is set properly
 *
 * @param command The inputted command
 * @return True if the command is correct, False otherwise
 */
bool acknowledgeCommand(command_t command)
{
  if (DEBUG_PRINT)
    printf("Acknowledging Command\n");

  if (command.command_type == command_types_t::NONE)
  {
    return false;
  }

  return true;
}

/**
 * TODO: Update this to current command sequence
 */
void sendDebug()
{
  // TODO: Include time since command sent
  Serial.flush();

  char ssr_buff[32];

  int reg_count = 0;
  for (int i : PUMP_ARRAY)
  {
    ssr_buff[reg_count++] = digitalRead(i) + '0';
  }

  for (int i : EXTRA_SSR_ARRAY)
  {
    ssr_buff[reg_count++] = digitalRead(i) + '0';
  }

  char pump_buff[5]; // use indexes 0—3. Index 4 holds null terminating char.
  sprintf(pump_buff, "%i", 9999);

  ssr_buff[26] = pump_buff[0];
  ssr_buff[27] = pump_buff[1];
  ssr_buff[28] = pump_buff[2];
  ssr_buff[29] = pump_buff[3];

  ssr_buff[30] = digitalRead(LED_POWER) + '0';
  ssr_buff[31] = digitalRead(LED_STATUS) + '0';

  char command_buff[31];
  sprintf(command_buff,
          ",%i,%i,%i,%i,%i,%i,%i\n",
          (int)debug_values.current_command.command_type,
          (int)debug_values.current_command.arg1,
          (int)debug_values.current_command.var1,
          (int)debug_values.current_command.arg2,
          (int)debug_values.current_command.var2,
          (int)debug_values.current_command.arg3,
          (int)debug_values.current_command.var3);

  Serial.write('D');
  Serial.write(ssr_buff);
  Serial.write(command_buff);
  Serial.flush();
}

void resetSystem()
{
  stepper_motor.on(0);
  stepper_motor.direction(0);

  for (int i : SSR_ARRAY)
  {
    if (i == LED_POWER)
      continue;

    pinMode(i, OUTPUT);
    digitalWrite(i, 0);
  }
}

void setup()
{
  digitalWrite(LED_POWER, 1);
  Serial.begin(baud_rate);

  Wire.begin();
  Wire.setClock(400000);

  neo_pixel.begin();
  if (stepper_motor.begin())
  {
    neo_pixel.setPixelColor(0, neo_pixel.Color(0, 150, 0));
  }
  else
  {
    neo_pixel.setPixelColor(0, neo_pixel.Color(150, 0, 0));
    Serial.println("Stepper Motor Not Started. ERROR.");
  }
  neo_pixel.show();

  resetSystem();

  digitalWrite(LED_POWER, 1);

  debug_values.enabled = false;
  debug_values.interval = false;
  debug_values.start_time = now();

  stepper_motor.direction(0);
};

int speed = 36;

void loop()
{
  if (Serial.available() > 0)
  {
    char rawSerial[64];
    readSerial(rawSerial);

    delay(100);

    command_t command = parseSerialCommand(rawSerial);

    noInterrupts();
    if (debug_values.enabled)
    {
      debug_values.current_command = command;
      debug_values.start_time = now();
    }
    interrupts();

    if (acknowledgeCommand(command))
    {
      Serial.write("ACK\n");
    }
    else
    {
      Serial.write("NAK\n");
      return;
    }

    delay(100);
    Serial.flush();

    if (runCommand(command))
    {
      Serial.write("FIN\n");
    }
    else
    {
      Serial.write("ERR\n");
    }

    delay(100);
    Serial.flush();
  }
}