#include "parse.h"
#include "execute.h"
#include "connector.h"
#include "gtest/gtest.h"


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
