#include <WiFi.h>

const char* ssid = "Matheus's Galaxy A23";
const char* password = "aygm9872";

void setup() {
  Serial.begin(115200);
  connectWiFi(ssid, password);
}

void loop() 
{
  // verifica se está conectado
  // caso contrário, espera um pouco e tenta novamente
  if(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    connectWiFi(ssid, password);
  }
  else
  {
    // código necessário
  }

}

// função padrão para configurar uma conexão WiFi
void connectWiFi(const char* ssid, const char* password)
{
  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.print("Conectado: "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}