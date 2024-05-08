# IR Detection

For the robot's movements, it was very important to know both the angle and the distance to the ball, so an IR ring was made with 12 IR receivers. However, before calculating these values, it was first necessary to obtain the pulse width of all TPS-58038s, which should be obtained during a ball emission cycle. However, this would imply a time of $833 \mu s \times  12$ sensors. Therefore, a better approach was to obtain a reading from all the sensors all at once and repeat this process during the cycle $(833 \mu s)$.
Once the method was chosen, the distance and angle had to be calculated. Nonetheless, each had different methods:

## Angle
To obtain the angle towards the ball, there were two main options:

- Define the angle as the value of the sensor with the greatest pulse width (12 possible values)
- Vector calculation (360°)

Since we wanted more precise values for the angle, we chose to use vector addition to get the resulting vector and hence, its angle. This was possible as each sensor had its unitary vector according to its position in the ring and the magnitud was the pulse width value. Therefore, after getting all sensor values in a cycle, the vectors where added using its x and y components, finally obtaining the angle as the inverse tangent of the result vector.


## Distance
For the distance, there were also different methods:

- Obtain the distance from the sensor with the greatest pulse width (its magnitud)
- Define the distance according to the number of phototransistors that detect the ball (active sensors)
- Vector calculation (resulting vector magnitud)
- Define the distance as the sum of the intensity measured by each sensor (combine method 1 and 2)

Even though vector calculation seemed to be the best method, we faced several issues, since it was not very consistent and the result value usually varied within a range from 20 to 40. In addition, the first and second method were also ineficient by themselves, resulting also in a small range of distance. Therefore, both options were combined, which provided the best results, getting a distance with a range from 20 to 100.

It is also important to mention, that we used the research of the [Yunit team (2017)](https://yunit.techblog.jp/archives/86422131.html
) as a reference. There were also several tests and conclusions done, which can be found in the following document: [IR Reasarch](https://docs.google.com/document/d/1eh5sBvWjGo1ZCivEp_j6wlTAJFp19s88jgZEiUuNRCA/edit?usp=sharing).


## Detect Possession
Ideally, we wanted to use the IR Ring to check if the robots had possesion of the ball. However during some tests, we discovered that due to the bright colors, the goals could reflect infrared light emitted by the ball. Therefore, the IR Ring was placed on the robot at a height slightly above the goals to avoid reflections. Nonetheless, this did not allow us to get precise distance measurements when the ball was very close, so we could'nt know if we had possesion or not. 
For this reason, another phototransistor was used with the only purpose to determine ball possesion. Similarly to the ring, the sensor counts the readings per cycle to determine the pulse width. However, to reduce noise and get more stable measurements, an [Exponential Moving Average (EMA)](https://github.com/luisllamasbinaburo/Arduino-SingleEmaFilter).

```cpp
int detect() {
  int pulseWidth = 0;
  int deltaPulseWidth = 5;

    const unsigned long startTime_us = micros();
    do {       
            filterAnalog.AddValue(analogRead(Constantes::analogo)); //Add value to filter
            if(filterAnalog.GetLowPass() > 700) {                   //Only consider the reading if the 
                pulseWidth += deltaPulseWidth;                      //infrarred emission is actually significant
            }
        
    } while((micros() - startTime_us) < 833);

  return pulseWidth;
}
```
