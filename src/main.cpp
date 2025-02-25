#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "RODRIGUES 2.4";
const char* password = "09221953";

// Cria o servidor assíncrono na porta 80
AsyncWebServer server(80);

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <title>ESP32 - WEBSERVER</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
      margin: 0;
      padding: 20px;
    }
    h1 {
      text-align: center;
      color: #333;
    }
    .dashboard {
      display: flex;
      justify-content: space-around;
      margin-bottom: 20px;
    }
    .card {
      background: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      text-align: center;
      width: 200px;
    }
    .card i {
      font-size: 40px;
      margin-bottom: 10px;
    }
    .card p {
      margin: 0;
      font-size: 18px;
    }
    .charts {
      display: flex;
      justify-content: space-around;
    }
    .chart-container {
      width: 45%;
      background: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    }
  </style>
</head>
<body>
  <h1>Leitura do Sensor DHT22</h1>
  
  <div class="dashboard">
    <div class="card">
      <i class="fas fa-thermometer-half"></i>
      <p>Temperatura: <span id="temp">Carregando...</span></p>
    </div>
    <div class="card">
      <i class="fas fa-tint"></i>
      <p>Umidade: <span id="hum">Carregando...</span></p>
    </div>
  </div>

  <div class="charts">
    <div class="chart-container">
      <canvas id="tempChart"></canvas>
    </div>
    <div class="chart-container">
      <canvas id="humChart"></canvas>
    </div>
  </div>

  <script>
    const tempCtx = document.getElementById('tempChart').getContext('2d');
    const humCtx = document.getElementById('humChart').getContext('2d');
  
    let minTemp = 20;
    let maxTemp = 40;
    let minHum = 70;
    let maxHum = 100;
  
    const tempChart = new Chart(tempCtx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Temperatura (°C)',
          data: [],
          borderColor: 'rgba(255, 99, 132, 1)',
          borderWidth: 2,
          fill: false
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          y: {
            beginAtZero: false, // Para que o gráfico se ajuste melhor ao valor
            suggestedMin: minTemp,    // Define o mínimo sugerido (ajustar conforme os dados)
            suggestedMax: maxTemp,   // Define o máximo sugerido (ajustar conforme os dados)
            ticks: {
              stepSize: 5 // Define a altura do intervalo de marcação no eixo Y
            }
          },
          x: {
            type: 'category',
            ticks: {
              maxRotation: 0, // Evita rotação excessiva no eixo X
              autoSkip: true, // Faz a legenda dos valores no eixo X ser adaptável
            }
          }
        }
      }
    });
  
    const humChart = new Chart(humCtx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Umidade (%)',
          data: [],
          borderColor: 'rgba(54, 162, 235, 1)',
          borderWidth: 2,
          fill: false
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          y: {
            beginAtZero: false,
            suggestedMin: minHum,  // Define o mínimo sugerido para umidade
            suggestedMax: maxHum,  // Define o máximo sugerido para umidade
            ticks: {
              stepSize: 10
            }
          },
          x: {
            type: 'category',
            ticks: {
              maxRotation: 0,
              autoSkip: true,
            }
          }
        }
      }
    });
  
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          const temp = data.temperatura;
          const hum = data.umidade;
          
          // Atualizando os dados exibidos
          document.getElementById('temp').innerHTML = temp + " ºC";
          document.getElementById('hum').innerHTML = hum + " %";
  
          // Atualizando os limites mínimos e máximos da temperatura
          if (temp < minTemp) minTemp = temp;
          if (temp > maxTemp) maxTemp = temp;
  
          // Atualizando os limites mínimos e máximos da umidade
          if (hum < minHum) minHum = hum;
          if (hum > maxHum) maxHum = hum;
  
          // Atualizando os gráficos com novos dados
          const now = new Date().toLocaleTimeString();
          tempChart.data.labels.push(now);
          tempChart.data.datasets[0].data.push(temp);
          if (tempChart.data.labels.length > 10) {
            tempChart.data.labels.shift();
            tempChart.data.datasets[0].data.shift();
          }
          tempChart.options.scales.y.suggestedMin = minTemp;
          tempChart.options.scales.y.suggestedMax = maxTemp;
          tempChart.update();
  
          humChart.data.labels.push(now);
          humChart.data.datasets[0].data.push(hum);
          if (humChart.data.labels.length > 10) {
            humChart.data.labels.shift();
            humChart.data.datasets[0].data.shift();
          }
          humChart.options.scales.y.suggestedMin = minHum;
          humChart.options.scales.y.suggestedMax = maxHum;
          humChart.update();
        })
        .catch(error => console.error("Erro ao atualizar dados: ", error));
    }
  
    setInterval(updateData, 5000); // Atualiza a cada 5 segundos
    window.onload = updateData; // Atualiza ao carregar a página
  </script>  
</body>
</html>
)rawliteral";

// Função que retorna os dados do sensor em formato JSON
String getSensorData() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Se falhar na leitura do sensor, retorna uma mensagem de erro
  if (isnan(temperatura) || isnan(umidade)) {
    return "{\"erro\": \"Erro ao ler o sensor DHT22\"}";
  }

  // Retorna os dados como JSON
  String jsonData = "{\"temperatura\": " + String(temperatura, 2) + ", \"umidade\": " + String(umidade, 2) + "}";
  return jsonData;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado ao Wifi");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  // Rota para servir a página HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", HTML_PAGE);
  });

  // Rota para servir os dados do sensor em formato JSON
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String sensorData = getSensorData();
    request->send(200, "application/json", sensorData);
  });

  // Inicia o servidor assíncrono
  server.begin();
}

void loop() {
  // O servidor é assíncrono, então não há necessidade de chamar algo aqui
  // O servidor já está processando as requisições automaticamente
}
