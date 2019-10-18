#include <Adafruit_Arcada.h>
Adafruit_Arcada arcada;

struct Machine;
struct MachineInternals;
struct Interpreter;
struct DiskDrive;
struct Overlay;
struct CoreDelegate;

struct Core {
    Machine *machine;
    MachineInternals *machineInternals;
    Interpreter *interpreter;
    DiskDrive *diskDrive;
    Overlay *overlay;
    CoreDelegate *delegate;
};

extern "C" {
  void core_init(Core *core);
}

struct Core core;

void setup() {
  Serial.begin(115200);
  
  arcada.begin();
  arcada.filesysBeginMSD();

  delay(1000);

    // Init screen with blue so we know its working
  arcada.displayBegin();
  arcada.fillScreen(ARCADA_BLUE);
  arcada.setBacklight(255);

  // Check we have a valid filesys
  if (arcada.filesysBegin()) {
    Serial.println("Found filesystem!");
  } else {
    arcada.haltBox("No filesystem found! For QSPI flash, load CircuitPython. For SD cards, format with FAT");
  }

  arcada.fillScreen(ARCADA_BLACK);
  core_init(&core);
  arcada.fillScreen(ARCADA_BLUE);
}

bool bl = false;
void loop() {
  if (bl)
    arcada.fillScreen(ARCADA_BLUE);
  else 
    arcada.fillScreen(ARCADA_BLACK);
  bl = !bl;
  delay(500);
}
