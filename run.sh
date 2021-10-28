#!/usr/bin/env bash

VERSION=$(date +%m%d%y)

docker build . -t mpfib:${VERSION} && \
	docker run --rm -i -t mpfib:${VERSION}
