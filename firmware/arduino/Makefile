.PHONY: compile_data_collector
compile_data_collector:
	arduino-cli compile -b arduino:mbed_nano:nano33ble data_collector/

.PHONY: upload_data_collector
upload_data_collector:
	sudo arduino-cli upload -b arduino:mbed_nano:nano33ble -p /dev/ttyACM0 data_collector/
