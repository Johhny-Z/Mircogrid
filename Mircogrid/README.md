## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Everytime you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.

To build this project for the ESP32, run:
```sh
source build_esp.sh
```

## Execute
To run the project, run:
```sh
./bin/sample_project
```

To flash the project onto the esp32, run:
```sh
idf.py -p $ESP_PORT flash
```

