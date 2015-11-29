////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Text overlay
//

#define OCTET_BULLET 1

#include "../../octet.h"

#include "Lsystem.h"





/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::lsystems app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();

  /*srand(time(NULL));
  float random = (float)((rand() % 10)*0.1);*/
}


