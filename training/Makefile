all: model.tflite model_tflite.cpp model_tflite.h

model.tflite:
	./model.py

model_tflite.cpp: model.tflite
	xxd -i $^ > $@

model_tflite.h: model_tflite.cpp
	echo "#ifndef MODEL_TFLITE_H" > $@
	echo "#define MODEL_TFLITE_H" >> $@
	echo "extern unsigned char model_tflite[];" >> $@
	echo "extern unsigned int model_tflite_len;" >> $@
	echo "#endif" >> $@

.PHONY: install
install: model_tflite.cpp model_tflite.h
	cp model_tflite.cpp model_tflite.h ../firmware/arduino/ml_app

.PHONY: clean
clean:
	rm model.tflite model_tflite.cpp model_tflite.h
