# ULPSM
Public Arduino Library for ULPSM (ultra-low power sensor module) or SDK (sensor development kit)

Included in this repository are a diagram of optional hook up, and instructions for reducing noise in the signals.

Notes and suggestions for Arduino library:
1) Before using temperature compensation look at the signal without compensation. The compensation algorithms are for health and safety levels only. For measuring air quality (low levels) the final temperature compensation will depend on many factors of your iplementation. Please contact SPEC sensor for advice on Air Quality temperature calibration for final implementation.
2) In the implementation of temperature compensation, be sure to pass the function a float value. If you pass an integer or a rounded float value, you will see large jumps in the concentration whenever the value changes, ie. from 24 C to 25 C. 
