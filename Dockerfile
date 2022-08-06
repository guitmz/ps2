FROM ps2dev/ps2dev

RUN apk add --no-cache build-base make gmp mpc1 mpfr readline-dev bash vim git tree cdrkit p7zip \
    && git clone https://github.com/mikeakohn/naken_asm.git \
    && cd naken_asm \
    && ./configure --enable-emotion-engine \
    && make \
    && cp naken_asm /usr/local/bin \
    && cp naken_util /usr/local/bin \
    && mkdir /src

ENV PS2SDKSRC $PS2DEV/ps2sdk
WORKDIR $PS2DEV

