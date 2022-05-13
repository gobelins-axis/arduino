# 💾 Arduino

## Description

Environment to create and upload scripts to Arduino cards.

## Getting Started

More information on the [Arduino CLI documentation](https://arduino.github.io/arduino-cli/0.20/getting-started/)

### Dependencies

-   Node.js - [Install](https://nodejs.org/en/download/)
-   Arduino CLI - [Install](https://arduino.github.io/arduino-cli/0.20/installation/)

### Installing

#### Create a configuration file

```bash
arduino-cli config init
```

#### Create a new sketch

```bash
# Replace "MySketch" by the sketch name you want
arduino-cli sketch new MySketch
```

#### Connect the board to your PC

```bash
# Update the local cache of available platforms and libraries
arduino-cli core update-index
```

Connect the Arduino board via USB

```bash
# Show devices connected to your computer
arduino-cli board list
```

Spot your arduino board and store the FQBN (if detected) somewhere, you'll need it soon.
If it's marked as Unknown, try to find it online by Arduino model / version or use this command :

```bash
arduino-cli board listall mkr
```

### Install the core for your board

Install the core you need depending on your board
Example: if your FQBN is "arduino:avr:nano" run

```bash
arduino-cli core install arduino:avr
```

### Configure build script

We created a node script to make it easier to compile and upload your code to Arduino.
Create a .env file at the root of the project and the FBQN string. example:

```env
FQBN=arduino:avr:nano:cpu=atmega328old
```

### Build

Start by installing node modules

```bash
npm install
```

Compile and upload to your board

```bash
# Replace "MySketch" by the sketch name you want to compile and upload
node build.js --name MySketch
```

Compile and upload to your board on file changes

```bash
# Replace "MySketch" by the sketch name you want to compile and upload
node build.js --name MySketch --watch
```

### Libraries

If a script fails because of a missing library, just check the libraries folder,
you will need to exact the library zip into ~/your_user_name/Documents/Arduino/Library.

## Authors

[@LPGeneret](https://twitter.com/LPGeneret)
[@sergebocancea](https://twitter.com/sergebocancea)
[@leochocolat](https://twitter.com/leochocolat)
