#define DOCTEST_CONFIG_IMPLEMENT  // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL
#include <doctest.h>
#include <mqtt_topic.h>

TEST_CASE("insantiation (empty)") {
  MqttTopic topic = MqttTopic();

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0);
}

TEST_CASE("insantiation (valid topic)") {
  MqttTopic topic = MqttTopic("house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 27); 
}

TEST_CASE("insantiation (late completion)") {
  MqttTopic topic = MqttTopic();
  topic.setTopic("house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 27); 
}

TEST_CASE("insantiation (leading '/')") {
  MqttTopic topic = MqttTopic("/house/frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (double '//')") {
  MqttTopic topic = MqttTopic("/house//frontroom/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (space in topic )") {
  MqttTopic topic = MqttTopic("/house/front room/tempurature");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '#')") {
  MqttTopic topic = MqttTopic("#");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 1); 
}

TEST_CASE("insantiation ('#' with char after)") {
  MqttTopic topic = MqttTopic("#a");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (use of '#' at end)") {
  MqttTopic topic = MqttTopic("house/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), true);
  REQUIRE_EQ(topic.getLength(), 17); 
}

TEST_CASE("insantiation (invalid of '#/' at end)") {
  MqttTopic topic = MqttTopic("house/frontroom/#/");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (several '#')") {
  MqttTopic topic = MqttTopic("house/#/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation ('+' and '#')") {
  MqttTopic topic = MqttTopic("house/+/frontroom/#");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (several '+')") {
  MqttTopic topic = MqttTopic("house/+/frontroom/+/size");

  REQUIRE_EQ(topic.isValidName(), false);
  REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+')") {
   MqttTopic topic = MqttTopic("+");

   REQUIRE_EQ(topic.isValidName(), false);
   REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("insantiation (single '+' at end)") {
   MqttTopic topic = MqttTopic("house/frontroom/+");

   REQUIRE_EQ(topic.isValidName(), false);
   REQUIRE_EQ(topic.getLength(), 0); 
}

TEST_CASE("== operator (same)") {
   MqttTopic topic1 = MqttTopic("house/frontroom/tempurature");
   MqttTopic topic2 = MqttTopic("house/frontroom/tempurature");

   REQUIRE_EQ(topic1 == topic2, true);
}

TEST_CASE("== operator (not same)") {
   MqttTopic topic1 = MqttTopic("house/frontroom/tempurature");
   MqttTopic topic2 = MqttTopic("house/frontroom/humidity");

   REQUIRE_EQ(topic1 == topic2, false);
}

TEST_CASE("== operator (valid #)") {
   MqttTopic topic1 = MqttTopic("house/frontroom/tempurature");
   MqttTopic topic2 = MqttTopic("house/frontroom/#");

   REQUIRE_EQ(topic1 == topic2, true);
}

TEST_CASE("== operator (+ operations)") {
   MqttTopic topic1 = MqttTopic("house/frontroom/tempurature");
   MqttTopic topic2 = MqttTopic("house/kitchen/tempurature");
   MqttTopic topic3 = MqttTopic("house/+/tempurature");
   MqttTopic topic4 = MqttTopic("house/+/humidity");
   MqttTopic topic5 = MqttTopic("house/+/cupboard/tempurature");

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