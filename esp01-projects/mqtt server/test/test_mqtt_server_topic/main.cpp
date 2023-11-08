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
  MqttServerTopic topic = MqttServerTopic("house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 27); 
}

TEST_CASE("insantiation (late completion)") {
  MqttServerTopic topic = MqttServerTopic();
  topic.setTopic("house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 27); 
}

TEST_CASE("insantiation (leading '/')") {
  MqttServerTopic topic = MqttServerTopic("/house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (double '//')") {
  MqttServerTopic topic = MqttServerTopic("/house//frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (space in topic )") {
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
  MqttServerTopic topic = MqttServerTopic("house/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 17); 
}

TEST_CASE("insantiation (invalid of '#/' at end)") {
  MqttServerTopic topic = MqttServerTopic("house/frontroom/#/");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (several '#')") {
  MqttServerTopic topic = MqttServerTopic("house/#/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation ('+' and '#')") {
  MqttServerTopic topic = MqttServerTopic("house/+/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (several '+')") {
  MqttServerTopic topic = MqttServerTopic("house/+/frontroom/+/size");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+')") {
   MqttServerTopic topic = MqttServerTopic("+");

   REQUIRE_EQ(topic.isValidName(), false);
   REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+' at end)") {
   MqttServerTopic topic = MqttServerTopic("house/frontroom/+");

   REQUIRE_EQ(topic.isValidName(), false);
   REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("== operator (same)") {
   MqttServerTopic topic1 = MqttServerTopic("house/frontroom/tempurature");
   MqttServerTopic topic2 = MqttServerTopic("house/frontroom/tempurature");

   REQUIRE_EQ(topic1 == topic2, true);
}

TEST_CASE("== operator (not same)") {
   MqttServerTopic topic1 = MqttServerTopic("house/frontroom/tempurature");
   MqttServerTopic topic2 = MqttServerTopic("house/frontroom/humidity");

   REQUIRE_EQ(topic1 == topic2, false);
}

TEST_CASE("== operator (valid #)") {
   MqttServerTopic topic1 = MqttServerTopic("house/frontroom/tempurature");
   MqttServerTopic topic2 = MqttServerTopic("house/frontroom/#");

   REQUIRE_EQ(topic1 == topic2, true);
}

TEST_CASE("== operator (+ operations)") {
   MqttServerTopic topic1 = MqttServerTopic("house/frontroom/tempurature");
   MqttServerTopic topic2 = MqttServerTopic("house/kitchen/tempurature");
   MqttServerTopic topic3 = MqttServerTopic("house/+/tempurature");
   MqttServerTopic topic4 = MqttServerTopic("house/+/humidity");
   MqttServerTopic topic5 = MqttServerTopic("house/+/cupboard/tempurature");

   REQUIRE_EQ(topic1 == topic2, false);
   REQUIRE_EQ(topic1 == topic3, true);
   REQUIRE_EQ(topic2 == topic3, true);
   REQUIRE_EQ(topic1 == topic4, false);
   REQUIRE_EQ(topic1 == topic5, false);
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