const client = mqtt.connect("ws://broker.emqx.io:8083/mqtt");
var indice = [];
var valor = [];
var i = 0;
var flag = 0;

client.on("connect", () => {
  console.log("Conectado ao broker");

  setInterval(()=>{
    const temp = (20 + Math.random()*5).toFixed(2);
    indice[i] = i++; 
    valor[i] = temp;
    if(flag==1) PlotaBatimentos();
    client.publish("Terroso/Temp", temp);
    console.log("Publicado:", temp);
  }, 1500);
});
client.subscribe("Terroso/Umid");
client.on("message", function (topic, message) {
 console.log(">>> Mensagem recebida do tópico:", topic);
 console.log(">>> Conteúdo bruto:", message.toString());
});

function Fechar()
{
  window.close();
}

function Ajuda()
{
  alert(`Esse dashboard faz o monitoramento de sinais vitais
    de batimentos cardíacos e níveis de saturação de oxigênio no sangue.
    Além disso, utiliza a conexão com um Broker MQTT para coletar as informações
    que vem de um ESP32 conectado a este mesmo Broker`);
}

function ConfigFlagBat()
{
  flag = 1;
}

var grafico_batimentos = null;
function PlotaBatimentos()
{  
  const dados_batimentos =
  {
    type:'bar',
    data:
    {
      labels: indice,
      datasets:
      [
        {
          label:'Dados do Batimentos Cardíacos',
          backgroundColor: 'rgba(251, 144, 144, 0.8)',
          //tension: 0.4,
          borderColor:'rgba(0,0,255,0.8)',
          data: valor
        }
      ]
    }
  };

  if(grafico_batimentos){
    grafico_batimentos.data.labels = indice;
    grafico_batimentos.data.datasets[0].data = valor;
    grafico_batimentos.update();
  }
  else{
    grafico_batimentos = new Chart(document.getElementById('plot1'),dados_batimentos);
  }
  
}


function atualiza_hora()
{
  document.getElementById("dia_hora").textContent = new Date();
}

setInterval(atualiza_hora, 500);


