#ifndef BME280_DATA_H
#define BME280_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Bme280Data {
    float humidity;
    float pressure;
    float temperature;
} Bme280Data;

Bme280Data readBME280();


#ifdef __cplusplus
}
#endif

#endif /* BME280_DATA_H */