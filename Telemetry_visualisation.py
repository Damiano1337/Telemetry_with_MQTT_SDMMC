import sys
import ssl
import json
import paho.mqtt.client as paho
import pyqtgraph as pg
from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6.QtCore import QTimer, pyqtSignal, QObject
import time
from pyqtgraph import AxisItem
import datetime


# Klasa do przekazywania danych z MQTT do GUI
class DataHandler(QObject):
    new_data = pyqtSignal(dict)

class TimeAxisItem(AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [datetime.datetime.fromtimestamp(value).strftime("%H:%M:%S") for value in values]


# Główne okno
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("MQTT Live Plot - 5 zmiennych")
        self.layout_widget = pg.GraphicsLayoutWidget()
        self.setCentralWidget(self.layout_widget)

        # 5 wykresów
        self.plot1 = self.layout_widget.addPlot(title="Voltage 1", axisItems={'bottom': TimeAxisItem(orientation='bottom')})
        self.plot2 = self.layout_widget.addPlot(title="Voltage 2", axisItems={'bottom': TimeAxisItem(orientation='bottom')})
        self.layout_widget.nextRow()
        self.plot3 = self.layout_widget.addPlot(title="Current 1", axisItems={'bottom': TimeAxisItem(orientation='bottom')})
        self.plot4 = self.layout_widget.addPlot(title="Current 2", axisItems={'bottom': TimeAxisItem(orientation='bottom')})
        self.layout_widget.nextRow()
        self.plot5 = self.layout_widget.addPlot(title="Hall sensor", axisItems={'bottom': TimeAxisItem(orientation='bottom')})

        self.t = list(range(100))  # t = czas / indeks
        self.voltage1 = [0 for _ in range(100)]
        self.voltage2 = [0 for _ in range(100)]
        self.current1 = [0 for _ in range(100)]
        self.current2 = [0 for _ in range(100)]
        self.hall = [0 for _ in range(100)]

        self.line1 = self.plot1.plot(self.t, self.voltage1, pen='r')
        self.line2 = self.plot2.plot(self.t, self.voltage2, pen='g')
        self.line3 = self.plot3.plot(self.t, self.current1, pen='b')
        self.line4 = self.plot4.plot(self.t, self.current2, pen='y')
        self.line5 = self.plot5.plot(self.t, self.hall, pen='m')

        # Obiekt przekazujący dane
        self.data_handler = DataHandler()
        self.data_handler.new_data.connect(self.update_plots)

    def update_plots(self, data):
        print("Aktualizacja wykresów z danymi:", data)

        # Aktualny czas w sekundach
        current_time = time.time()

        # Usuwamy stare dane starsze niż 60 sekund
        self.t.append(current_time)
        self.voltage1.append(float(data.get('v1', 0)))
        self.voltage2.append(float(data.get('v2', 0)))
        self.current1.append(float(data.get('c1', 0)))
        self.current2.append(float(data.get('c2', 0)))
        self.hall.append(float(data.get('hall', 0)))

        # Trzymamy tylko dane z ostatniej minuty
        while self.t and self.t[0] < current_time - 60:
            self.t.pop(0)
            self.voltage1.pop(0)
            self.voltage2.pop(0)
            self.current1.pop(0)
            self.current2.pop(0)
            self.hall.pop(0)

        # Aktualizujemy wykresy
        self.line1.setData(self.t, self.voltage1)
        self.line2.setData(self.t, self.voltage2)
        self.line3.setData(self.t, self.current1)
        self.line4.setData(self.t, self.current2)
        self.line5.setData(self.t, self.hall)

        # Auto zoom na X (żeby wykres pokazywał 60 sekund)
        for plot in [self.plot1, self.plot2, self.plot3, self.plot4, self.plot5]:
            plot.setXRange(current_time - 60, current_time)
            plot.enableAutoRange(axis='y')  # tylko Y automatyczne

# Funkcje MQTT
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_message(client, userdata, msg):
    print("Received message:")
    try:
        payload = json.loads(msg.payload.decode())
        window.data_handler.new_data.emit(payload)  # wysyłamy dane do GUI
    except Exception as e:
        print(f"Błąd dekodowania wiadomości: {e}")

# Konfiguracja klienta MQTT
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

client.tls_set(tls_version=ssl.PROTOCOL_TLS)

client.username_pw_set("dataServer", "Hydrive1")
client.connect("6aa9c1de47b0404e82629f1961517374.s1.eu.hivemq.cloud", 8883)

client.subscribe("esp32/pub", qos=1)

# Start aplikacji
app = QApplication(sys.argv)
window = MainWindow()
window.show()

# Uruchamiamy MQTT w osobnym wątku
client.loop_start()

app.exec()

# Po zamknięciu okna zatrzymujemy MQTT
client.loop_stop()
client.disconnect()
