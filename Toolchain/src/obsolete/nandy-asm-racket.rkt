#lang racket

(require "nandy-asm-core.rkt")
(require racket/cmdline)

(command-line 
    #:once-each
    #:args (file-in file-out)
    (assemble file-in file-out))