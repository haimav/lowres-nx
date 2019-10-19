#include <Adafruit_Arcada.h>
//#include <Adafruit_ZeroDMA.h>

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

  arcada.fillScreen(ARCADA_CYAN);
  arcada.infoBox("Loading run.nx into FLASH memory...", 0);
  char *nxfile = (char *)arcada.writeFileToFlash("/run.nx", 0x40000);  // TODO: is this the correct address?
  Serial.printf(" into address $%08x", (uint32_t)nxfile);
  if ((uint32_t)nxfile== 0) {
    Serial.println("Unable to load file into FLASH, maybe too large?");
  }
 
  core_init(&core);
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
