#include <iostream>
#include <memory>
#include "Graphics/CMentalWindow.h"

int main(int argc, const char* argv[])
{
  MentalGraphics::MentalWindow window;
  window.mainLoop();
  return 0;
}
