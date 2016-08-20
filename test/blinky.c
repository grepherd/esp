#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"

//Get wifi ssid and password
#include "ssid.h"

struct espconn pHTTPServer;
esp_tcp tcp;

static const int pin = 2;
static volatile os_timer_t some_timer;

void some_timerfunc(void *arg)
{
  //Do blinky stuff
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
    {
      // set gpio low
      gpio_output_set(0, (1 << pin), 0, 0);
    }
  else
    {
      // set gpio high
      gpio_output_set((1 << pin), 0, 0, 0);
    }
}

void wifi_connect() {
  struct station_config stationConf;

  wifi_set_opmode(STATION_MODE);
  os_memcpy(&stationConf.ssid, WIFI_SSID, 32);
  os_memcpy(&stationConf.password, WIFI_PASSWORD, 32);
  wifi_station_set_config(&stationConf);
  wifi_station_connect();
}

void ICACHE_FLASH_ATTR http_recvcb(void *arg, char *pdata, unsigned short len) {
  os_printf("%s\n\r", pdata);

  /*    char data[256] = "HTTP/1.1 404 Not Found	     \
	Content-type: text/html  \
	Content-length: 135        \
	<html><head><title>Not Found</title></head><body> \
	Sorry, the object you requested was not found. \
	</body></html>\0"; 
  */

  char data[256] = "Ok\n\r\0";

  //returns 0 on success
  if (espconn_sent((struct espconn *) arg, data, 256) != ESPCONN_OK) {
    os_printf("Response failed\n\r");
  } else {
    os_printf("Response sent\n\r");
  }
  /*if (espconn_disconnect((struct espconn *) arg) != ESPCONN_OK) {
    os_printf("Disconnect failed\n\r");
    } else {
    os_printf("Disconnected\n\r");
    }*/
}

int http_disconnectcb(struct espconn *espconn) {

}

void ICACHE_FLASH_ATTR server_connectcb(void *arg)
{
  int i;
  struct espconn *pespconn = (struct espconn *)arg;

  //espconn's have a extra flag you can associate extra information with a connection.
  //pespconn->reverse = my_http;

  //Let's register a few callbacks, for when data is received or a disconnect happens.
  espconn_regist_recvcb( pespconn, http_recvcb );
  espconn_regist_disconcb( pespconn, NULL );
}



void ICACHE_FLASH_ATTR user_init()
{
  uart_div_modify(0, UART_CLK_FREQ / (115200));

  os_printf("Boot complete\n\r");
	
  // init gpio sussytem
  gpio_init();

  wifi_connect();

  //Initialize the ESPConn
  espconn_create( &pHTTPServer );
  pHTTPServer.type = ESPCONN_TCP;
  pHTTPServer.state = ESPCONN_NONE;

  //Make it a TCP connection.
  pHTTPServer.proto.tcp = &tcp;
  pHTTPServer.proto.tcp->local_port = 80;

  //"httpserver_connectcb" gets called whenever you get an incoming connetion.
  espconn_regist_connectcb(&pHTTPServer, server_connectcb);

  //Start listening!
  espconn_accept(&pHTTPServer);

  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  gpio_output_set(0, 0, (1 << pin), 0);

  // setup timer (500ms, repeating)
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
  os_timer_arm(&some_timer, 2000, 1);
}
