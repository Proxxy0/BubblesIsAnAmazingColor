import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(5, GPIO.OUT)
GPIO.setup(6, GPIO.OUT)
GPIO.setup(18, GPIO.OUT)

GPIO.output(5, GPIO.HIGH)
time.sleep(0.5)
GPIO.output(5, GPIO.LOW)

GPIO.output(6, 1)
time.sleep(0.5)
GPIO.output(6, 0)

pwm = GPIO.PWM(18,440)
pwm.start(50)
time.sleep(3)
pwm.stop()

GPIO.cleanup()
