# IR Detection

Para los movimientos del robot, fue muy importante conocer tanto el ángulo como la distancia a la pelota, por lo que se implementó un aro IR formado por 12 receptores IR  . Sin embargo, antes de calcular estos valores, primero fue necesario obtener el ancho de pulso de todos los TPS-58038. Para esto, se podría obtener el ancho de pulso de cada sensor durante un ciclo de emisión de la pelota, lo cual implicaría un tiempo de 833s12 sensores. No obstante, se optó por obtener una lectura de todos los sensores y repetir este proceso durante el ciclo (833s). 
Por otro lado, para calcular la distancia y el ángulo, se probaron los siguientes métodos:

### Angle
- Definir el ángulo en base al sensor con mayor ancho de pulso (12 posibles valores)
- Cálculos vectoriales (360°)
Para el ángulo se decidió usar cálculo vectorial para tener medidas más precisas. Por lo tanto, cada sensor tiene sus componentes de vector unitario de acuerdo a su posición en el aro y su magnitud es el ancho de pulso. De esta manera, se puede realizar una suma de los vectores y obtener el ángulo final.


### Distance
- Obtener la distancia del sensor con mayor ancho de pulso (su magnitud)
- Definir la distancia en base a cuántos fototransistores detectan la pelota (sensores activos)
- Cálculo vectorial
- Definir la distancia como la suma de la intensidad capturada por cada sensor (combinación entre el método uno y dos) 
A pesar de que el cálculo vectorial podría aparentar ser el mejor método, solo presentaba un rango de 20 a 40. Por lo tanto, se optó por el último método, que presentó un rango de distancia de 20 a 100.


## Detection
Durante pruebas, se descubrió que las porterías en ocasiones reflejaban la luz infrarroja, por lo que el aro IR se encontraba a una altura superior a las porterías. Sin embargo, esto no permitía tener mediciones precisas de distancia cuando la pelota estaba muy cerca. Por este motivo, fue necesario implementar otro fototransistor, el cual de manera similar al aro, hace un conteo de las lecturas por ciclo de la pelota para determinar el ancho de pulso. Adicionalmente, se empleó un filtro [Exponential Moving Average (EMA)](
    https://github.com/luisllamasbinaburo/Arduino-SingleEmaFilter
) para reducir el ruido y obtener medidas más estables tanto en el detector como el aro. 
https://github.com/luisllamasbinaburo/Arduino-SingleEmaFilter