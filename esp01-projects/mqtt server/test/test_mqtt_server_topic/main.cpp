#define DOCTEST_CONFIG_IMPLEMENT  // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL
#include <doctest.h>
#include <mqtt_server_topic.h>

TEST_CASE("insantiation (empty)") {
  MqttServerTopic topic = MqttServerTopic();

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0);
}

TEST_CASE("insantiation (valid topic)") {
  MqttServerTopic topic = MqttServerTopic("house/front room/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 28); 
}

TEST_CASE("insantiation (late completion)") {
  MqttServerTopic topic = MqttServerTopic();
  topic.setTopic("house/front room/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 28); 
}

TEST_CASE("insantiation (leading '/')") {
  MqttServerTopic topic = MqttServerTopic("/house/front room/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '#')") {
  MqttServerTopic topic = MqttServerTopic("#");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 1); 
}

TEST_CASE("insantiation ('#' with char after)") {
  MqttServerTopic topic = MqttServerTopic("#a");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (use of '#' at end)") {
  MqttServerTopic topic = MqttServerTopic("house/front room/#");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 18); 
}

TEST_CASE("insantiation (invalid of '#/' at end)") {
  MqttServerTopic topic = MqttServerTopic("house/front room/#/");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+')") {
  MqttServerTopic topic = MqttServerTopic("+");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+' at end)") {
  MqttServerTopic topic = MqttServerTopic("house/front room/+");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
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