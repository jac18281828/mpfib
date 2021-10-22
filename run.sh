#!/usr/bin/env bash

VERSION=$(date +%s)

docker build . -t mpfib:${VERSION} && \
	docker run --rm -i -t mpfib:${VERSION}
