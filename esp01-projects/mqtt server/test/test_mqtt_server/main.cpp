#define DOCTEST_CONFIG_IMPLEMENT  // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL

#include <doctest.h>
#include <mqtt_server.h>

TEST_CASE("insantiation") {
  MqttServer server = MqttServer({"192", "168", "4", "2"}, MQTT_BROKER_PORT);

  //REQUIRE_EQ(, true);
}

int main(int argc, char **argv)
{
  doctest::Context context;

  // BEGIN:: PLATFORMIO REQUIRED OPTIONS
  context.setOption("success", true);     // Report successful tests
  context.setOption("no-exitcode", true); // Do not return non-zero code on failed test case
  // END:: PLATFORMIO REQUIRED OPTIONS

  // YOUR CUSTOM DOCTEST OPTIONS
  context.applyCommandLine(argc, argv);
  return context.run();
}