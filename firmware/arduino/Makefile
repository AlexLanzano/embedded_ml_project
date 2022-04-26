.PHONY: compile_data_collector
compile_data_collector:
	arduino-cli compile -b arduino:mbed_nano:nano33ble data_collector/

.PHONY: upload_data_collector
upload_data_collector:
	sudo arduino-cli upload -b arduino:mbed_nano:nano33ble -p /dev/ttyACM0 data_collector/

.PHONY: compile_ml_app
compile_ml_app:
	arduino-cli compile -b arduino:mbed_nano:nano33ble ml_app/

.PHONY: upload_ml_app
upload_ml_app:
	sudo arduino-cli upload -b arduino:mbed_nano:nano33ble -p /dev/ttyACM0 ml_app/
