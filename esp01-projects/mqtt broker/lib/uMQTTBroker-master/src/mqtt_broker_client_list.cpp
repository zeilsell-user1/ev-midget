#include "mqtt/mqtt_server_client_list.h"

MqttServerClientList::MqttServerClientList()
{

}

void MqttServerClientList::cleanupClientCons()
{
	MQTT_ClientCon *clientcon, *clientcon_tmp;
	for (clientcon = clientcon_list; clientcon != NULL;)
	{
		clientcon_tmp = clientcon;
		clientcon = clientcon->next;
		if (clientcon_tmp->pCon->state == ESPCONN_CLOSE)
		{
			espconn_delete(clientcon_tmp->pCon);
			MQTT_server_deleteClientCon(clientcon_tmp);
		}
	}
}