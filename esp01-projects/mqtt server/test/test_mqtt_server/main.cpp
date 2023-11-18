#define DOCTEST_CONFIG_IMPLEMENT // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL

#include <doctest.h>
#include <mqtt_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "espconn.h"

signed char espconn_accept(struct espconn *espconn) { return 0; }
signed char espconn_connect(struct espconn *espconn) { return 0; }
signed char espconn_disconnect(struct espconn *espconn) { return 0; }
signed char espconn_abort(struct espconn *espconn) { return 0; }

signed char espconn_regist_sentcb(
    struct espconn *espconn,
    espconn_sent_callback sent_cb) { return 0; }

signed char espconn_regist_recvcb(
    struct espconn *espconn,
    espconn_recv_callback recv_cb) { return 0; }

signed char espconn_regist_connectcb(
    struct espconn *espconn,
    espconn_connect_callback connect_cb) { return 0; }

signed char espconn_regist_reconcb(
    struct espconn *espconn,
    espconn_reconnect_callback recon_cb) { return 0; }

signed char espconn_regist_disconcb(
    struct espconn *espconn,
    espconn_connect_callback discon_cb) { return 0; }

TEST_CASE("insantiation of the server")
{
    ip_addr_t ipAddress;
    IP4_ADDR(&ipAddress, 192, 168, 4, 2);
    MqttServer mqttClient = MqttServer(ipAddress, MQTT_BROKER_PORT);

    // REQUIRE_EQ(, true);
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
