# syntax=docker/dockerfile:1.3.0-labs
FROM ghcr.io/greyltc-org/firmware-builder:20220216.0.104 as compile
ARG EXAMPLE=DropIn
COPY src /ATmega328PB/src
COPY Examples/${EXAMPLE}/${EXAMPLE}.ino /ATmega328PB/src/${EXAMPLE}.cpp
RUN <<EOF
#!/usr/bin/env bash
echo 'build_flags = "-D ARDUINO_AVR_USTEPPER_S"' >> /ATmega328PB/platformio.ini
pio run -d /ATmega328PB
EOF

FROM scratch AS export
ENV BOARD=ATmega328PB
COPY --from=compile /${BOARD}/.pio/build/${BOARD}/firmware.hex /${BOARD}.hex
