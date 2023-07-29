#lang racket

(define assemble
  (lambda (fname)
    (port->string (open-input-file fname #:mode 'text))))