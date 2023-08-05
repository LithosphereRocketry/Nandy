#lang racket
(define linebrk-token "\n")
(define comment-token "#")
(define label-token ":")
(define bytesel-token "$")

; == AST expressions here ==
; Abstract constructs
(struct text-exp (str) #:transparent)
(struct list-exp (contents) #:transparent)
(struct label-exp (label target) #:transparent)

; Macros
(struct @include-exp (path) #:transparent)
(struct @define-exp (sym val) #:transparent)

; Real instructions
(struct none-exp () #:transparent)
(struct nop-exp () #:transparent)
(struct rd-exp (reg) #:transparent)
(struct wr-exp (reg) #:transparent)
(struct sw-exp (reg) #:transparent)
(struct ja-exp () #:transparent)
(struct jar-exp () #:transparent)
(struct sig-exp (num) #:transparent)
(struct isp-exp (num) #:transparent)
(struct add-exp (reg) #:transparent)
(struct lda-exp (target) #:transparent)
(struct lds-exp (target) #:transparent)
(struct stra-exp (target) #:transparent)
(struct strs-exp (target) #:transparent)
(struct rdi-exp (num) #:transparent)
(struct addi-exp (num) #:transparent)
(struct subi-exp (num) #:transparent)
(struct j-exp (label) #:transparent)
(struct jif-exp (sig label) #:transparent)

; Pseudoinstructions
(struct move-exp (from to) #:transparent)
(struct swap-exp (a b) #:transparent)
(struct call-exp (label) #:transparent)

(define noop (lambda (a) a))

(define parse-fpath
  (lambda (a) a)) ; TODO: make this resolve paths in a sensible way

(define parse-reg
  (lambda (name)
    (case name
      [("acc") 'acc]
      [("x" "dx" "dl") 'dx]
      [("y" "dy" "dh") 'dy]
      [("sp") 'sp]
      [("io") 'io]
      [else (raise (string-append "Unrecognized register: " name))])))

(struct bytenum (symbol index) #:transparent)
(define parse-number
  (lambda (rep)
    (or (string->number rep)
        (and (string-contains? rep bytesel-token)
             (let* ([rspl (string-split rep bytesel-token)]
                    [bnum (string->number (cadr rspl))])
               (if bnum
                   (bytenum (parse-number (car rspl)) bnum)
                   (raise "Byte index must be a number"))))
        rep)))  ; if it's not a numeric literal it's a symbol, save a string & deal with it later

(define parse-label
  (lambda (lbl)
    lbl))

(define parse-sigout
  (lambda (name)
    (case name
      [("brk" "0") 0]
      [else (raise (string-append "Unrecognized output signal: " name))])))

(define parse-sigin
  (lambda (name)
    (case name
      [("carry" "0") 0]
      [else (raise (string-append "Unrecognized input signal: " name))])))

; == Text -> instruction translation here
(struct idesc (generator arg-parsers))
(define instructions
  (make-immutable-hash
   (list (cons "@define" (idesc @define-exp (list parse-label parse-number)))
         (cons "@include" (idesc @include-exp (list parse-fpath)))
         (cons "nop" (idesc nop-exp '()))
         (cons "rd" (idesc rd-exp (list parse-reg)))
         (cons "wr" (idesc wr-exp (list parse-reg)))
         (cons "sw" (idesc sw-exp (list parse-reg)))
         (cons "ja" (idesc ja-exp '()))
         (cons "jar" (idesc jar-exp '()))
         (cons "sig" (idesc sig-exp (list parse-sigout)))
         (cons "isp" (idesc isp-exp (list parse-number)))
         (cons "add" (idesc add-exp (list parse-reg)))
         (cons "lda" (idesc lda-exp (list parse-number)))
         (cons "lds" (idesc lds-exp (list parse-number)))
         (cons "stra" (idesc stra-exp (list parse-number)))
         (cons "strs" (idesc strs-exp (list parse-number)))
         (cons "rdi" (idesc rdi-exp (list parse-number)))
         (cons "addi" (idesc addi-exp (list parse-number)))
         (cons "subi" (idesc subi-exp (list parse-number)))
         (cons "j" (idesc j-exp (list parse-label)))
         (cons "jif" (idesc jif-exp (list parse-sigin parse-label)))
         
         (cons "call" (idesc call-exp (list parse-label)))
         (cons "move" (idesc move-exp (list parse-reg parse-reg))))))


; File handling
(define load-file
  (lambda (fname)
    (let* ([file (open-input-file fname #:mode 'text)]
           [data (port->string file)])
      (close-input-port file)
      data)))

(define save-binfile!
  (lambda (fname bdata)
    (let* ([file (open-output-file fname #:mode 'binary #:exists 'replace)])
      (write-bytes bdata file)
      (close-output-port file))))

(define organize-str
  (lambda (codestr)
    (let* ([lines (string-split codestr linebrk-token)] ; break into lines
           [lines-csplit (map (lambda (l) (string-split l comment-token #:trim? #f)) lines)]
           [lines-nc (map car (filter (lambda (item) (not (null? item)))
                                      lines-csplit))] ; remove comments
           [lines-norm (filter non-empty-string? (map string-normalize-spaces lines-nc))]) ; remove whitespace
      (list-exp (map text-exp lines-norm)))))

(define parse-str
  (lambda (str)
    (let recurse ([linetok (string-split str label-token #:trim? #f)])
      (if (> (length linetok) 1)
          (label-exp (car linetok) (recurse (cdr linetok)))
          (let ([tok (map string-normalize-spaces (string-split (car linetok)))])
            (if (null? tok)
                (none-exp)
                (parse-inst (car tok) (cdr tok))))))))

(define parse-inst
  (lambda (istr args)
    (if (hash-has-key? instructions istr)
        (let ([instruct (hash-ref instructions istr)])
          (if (= (length (idesc-arg-parsers instruct)) (length args))
              (apply (idesc-generator instruct)
                     (let recurse ([to-parse args]
                                   [to-apply (idesc-arg-parsers instruct)]
                                   [completed '()])
                       (if (null? to-parse)
                           (reverse completed)
                           (recurse (cdr to-parse)
                                    (cdr to-apply)
                                    (cons ((car to-apply) (car to-parse)) completed)))))
              (raise (string-append "Wrong number of arguments for '" istr "' in: "
                                    (string-join (cons istr args))))))
        (raise (string-append "Unrecognized instruction: " istr)))))

(define parse
  (lambda (exp)
    (cond [(text-exp? exp) (parse (parse-str (text-exp-str exp)))]
          [(list-exp? exp) (list-exp (map parse (list-exp-contents exp)))]
          [(label-exp? exp) (label-exp (label-exp-label exp) (parse (label-exp-target exp)))]
          [else exp])))

; Macro definitions go here
(define macro-expand
  (lambda (exp)
    (cond [(@include-exp? exp) (expand-file (@include-exp-path exp))]
          [(call-exp? exp) (list-exp (list (wr-exp 'dy)
                                           (rdi-exp (bytenum (call-exp-label exp) 0))
                                           (wr-exp 'dx)
                                           (rdi-exp (bytenum (call-exp-label exp) 1))
                                           (sw-exp 'dy)
                                           (jar-exp)))]
          [(move-exp? exp) (cond [(eq? (move-exp-to exp) (move-exp-from exp)) (none-exp)]
                                 [(eq? (move-exp-to exp) 'acc) (rd-exp (move-exp-from exp))]
                                 [(eq? (move-exp-from exp) 'acc) (wr-exp (move-exp-to exp))]
                                 [else (list-exp (list (sw-exp (move-exp-to exp))
                                                       (rd-exp (move-exp-from exp))
                                                       (sw-exp (move-exp-to exp))))])]
          [(list-exp? exp) (list-exp (map macro-expand (list-exp-contents exp)))]
          [(label-exp? exp) (label-exp (label-exp-label exp) (macro-expand (label-exp-target exp)))]
          [else exp])))

(define expand-file
  (lambda (fname)
    (macro-expand (parse (organize-str (load-file fname))))))

; Turns any AST into a flat list of expressions in order, maybe with labels applied
(define flatten
  (lambda (exp)
    (cond [(list-exp? exp) (let recurse ([rem-list (list-exp-contents exp)]
                                          [built-list '()])
                              (if (null? rem-list)
                                  (list-exp built-list)
                                  (recurse (cdr rem-list)
                                           (append built-list
                                                   (list-exp-contents (flatten (car rem-list)))))))]
          [(label-exp? exp) (let ([result (list-exp-contents (flatten (label-exp-target exp)))])
                              (if (null? (cdr result))
                                  (list-exp (list (label-exp (label-exp-label exp) (car result))))
                                  ; if a label is applied to a macro expansion, apply it to the first element
                                  (list-exp (cons (label-exp (label-exp-label exp) (car result)) (cdr result)))))]
          [else (list-exp (list exp))])))

; Number of bytes in a given expression; only accepts primitives
(define exp-length
  (lambda (exp)
    (cond [(nop-exp? exp) 1]
          [(rd-exp? exp) 1]
          [(wr-exp? exp) 1]
          [(sw-exp? exp) 1]
          [(ja-exp? exp) 1]
          [(jar-exp? exp) 1]
          [(sig-exp? exp) 1]
          [(isp-exp? exp) 1]
          [(add-exp? exp) 1]
          [(lda-exp? exp) 1]
          [(lds-exp? exp) 1]
          [(stra-exp? exp) 1]
          [(strs-exp? exp) 1]
          [(rdi-exp? exp) 2]
          [(addi-exp? exp) 2]
          [(subi-exp? exp) 2]
          [(j-exp? exp) 2]
          [(jif-exp? exp) 2]
          [else (raise (string-append "Expression " (format "~a" exp) " has no defined length"))])))

(define byte->sbyte
  (lambda (num)
    (if (> num 127)
        (- num 256)
        num)))

(define sbyte->byte
  (lambda (num)
    (if (< num 0)
        (+ num 256)
        num)))

; Gets the nth byte of a number
(define get-bnum
  (lambda (num bn)
    (byte->sbyte (bitwise-and (arithmetic-shift num (* bn -8)) 255))))
  
; Converts a symbolic value into a real number
(define get-value
  (lambda (rep ltab)
    (cond [(number? rep) rep]
          [(string? rep) (hash-ref ltab rep)]
          [(bytenum? rep) (get-bnum (get-value (bytenum-symbol rep) ltab)
                                    (bytenum-index rep))]
          [else (raise "Invalid type for get_value")])))
  
; Converts a list-exp into a list of primitive expressions and a hash of label locations
(struct labeled-program (ltab ilist) #:transparent)
(define resolve-labels
  (lambda (lexp)
    (if (list-exp? lexp)
        (let recurse ([exps (list-exp-contents lexp)]
                      [ltab (make-immutable-hash)]
                      [clean-exps '()]
                      [index 0])
          (if (null? exps)
              (labeled-program ltab (reverse clean-exps))
              (cond [(none-exp? (car exps)) (recurse (cdr exps) ltab clean-exps index)]
                    [(list-exp? (car exps)) (raise "Can only resolve labels on a flattened program")]
                    [(@define-exp? (car exps)) (recurse (cdr exps)
                                                        (hash-set ltab
                                                                  (@define-exp-sym exp)
                                                                  (get-value (@define-exp-val exp) ltab index)))]
                    [(label-exp? (car exps)) (recurse (cons (label-exp-target (car exps)) (cdr exps))
                                                      (hash-set ltab (label-exp-label (car exps)) index)
                                                      clean-exps index)]
                    [else (recurse (cdr exps) ltab (cons (car exps) clean-exps) (+ index (exp-length (car exps))))])))
        (raise "Can only resolve labels on list expressions"))))

; Makes symbolic values into real numbers
; Note: doesn't touch instructions it doesn't know how to handle, doesn't warn if it does so
; because most instructions don't need to be modified by this
(define resolve-values
  (lambda (lpgrm)
    (let ([ltab (labeled-program-ltab lpgrm)]
          [ilist (labeled-program-ilist lpgrm)])
      (map (lambda (inst)
             (cond [(isp-exp? inst) (isp-exp (get-value (isp-exp-num inst) ltab))]
                   [(lda-exp? inst) (lda-exp (get-value (lda-exp-target inst) ltab))]
                   [(lds-exp? inst) (lds-exp (get-value (lds-exp-target inst) ltab))]
                   [(stra-exp? inst) (stra-exp (get-value (stra-exp-target inst) ltab))]
                   [(strs-exp? inst) (strs-exp (get-value (strs-exp-target inst) ltab))]
                   [(rdi-exp? inst) (rdi-exp (get-value (rdi-exp-num inst) ltab))]
                   [(addi-exp? inst) (addi-exp (get-value (addi-exp-num inst) ltab))]
                   [(subi-exp? inst) (subi-exp (get-value (subi-exp-num inst) ltab))]
                   [(j-exp? inst) (j-exp (get-value (j-exp-label inst) ltab))]
                   [(jif-exp? inst) (jif-exp (jif-exp-sig inst) (get-value (jif-exp-label inst) ltab))]
                   [else inst])) ilist))))

(define reg->bits
  (lambda (reg)
    (case reg
      [(acc) (raise "acc register is not valid as register move")]
      [(sp) #b00]
      [(io) #b01]
      [(dx) #b10]
      [(dy) #b11]
      [else (raise reg)])))

(define regmath->bits
  (lambda (reg)
    (case reg
      [(dx) #b000000]
      [(dy) #b100000]
      [else (raise "Invalid register for math operation")])))

(define num->4bi
  (lambda (num)
    (if (and (exact-integer? num) (>= num -8) (< num 8)) (bitwise-and #xF num)
        (raise (format "~a is not a valid 4-bit immediate" num)))))

(define num->u3bi
  (lambda (num)
    (if (and (exact-integer? num) (>= num 0) (< num 8)) num
        (raise (format "~a is not a valid unsigned 3-bit immediate" num)))))

(define num->8bi
  (lambda (num)
    (if (and (exact-integer? num) (>= num -128) (< num 128)) num
        (raise (format "~a is not a valid 8-bit immediate" num)))))

; Instruction binary conversion goes here
(define inst->blist
  (lambda (inst loc)
    (let ([rep (cond [(nop-exp? inst) (list #b00000000)]
                     [(rd-exp? inst) (list (bitwise-ior #b00000100 (reg->bits (rd-exp-reg inst))))]
                     [(wr-exp? inst) (list (bitwise-ior #b00001000 (reg->bits (wr-exp-reg inst))))]
                     [(sw-exp? inst) (list (bitwise-ior #b00001100 (reg->bits (sw-exp-reg inst))))]
                     [(ja-exp? inst) (list #b00010000)]
                     [(jar-exp? inst) (list #b00010100)]
                     [(sig-exp? inst) (list (bitwise-ior #b00011000 (num->u3bi (sig-exp-num inst))))]
                     [(isp-exp? inst) (list (bitwise-ior #b00110000 (num->4bi (isp-exp-num inst))))]
                     [(add-exp? inst) (list (bitwise-ior #b01010100 (regmath->bits (add-exp-reg inst))))]
                     [(lda-exp? inst) (list (bitwise-ior #b10000000 (num->4bi (lda-exp-target inst))))] 
                     [(lds-exp? inst) (list (bitwise-ior #b10010000 (num->4bi (lds-exp-target inst))))]
                     [(stra-exp? inst) (list (bitwise-ior #b10100000 (num->4bi (stra-exp-target inst))))]
                     [(strs-exp? inst) (list (bitwise-ior #b10110000 (num->4bi (strs-exp-target inst))))]
                     [(rdi-exp? inst) (list #b11000000 (num->8bi (rdi-exp-num inst)))]
                     [(addi-exp? inst) (list #b11010100 (num->8bi (addi-exp-num inst)))]
                     [(subi-exp? inst) (list #b11010110 (num->8bi (subi-exp-num inst)))]
                     [(j-exp? inst) (let ([offset (- (j-exp-label inst) (+ loc 1))])
                                      (list (bitwise-ior #b11100000 (num->4bi (get-bnum offset 1)))
                                            (num->8bi (get-bnum offset 0))))]
                     [(jif-exp? inst) (let ([offset (- (jif-exp-label inst) (+ loc 1))])
                                        (list (bitwise-ior #b11110000 (num->u3bi (jif-exp-sig inst)))
                                              (num->8bi offset)))]
                     [else (raise "Could not convert to binary")])])
      (if (= (length rep) (exp-length inst))
          rep
          (raise "Instruction misreported its size")))))

(define total-before
  (lambda (lon)
    (let recurse ([lrem lon]
                  [done '(0)])
      (if (null? lrem)
          (reverse (cdr done)) ; skip the sum of all so the length is the same
          (recurse (cdr lrem) (cons (+ (car done) (car lrem)) done))))))

(define build-binary
  (lambda (ilist)
    (let* ([lengths (map exp-length ilist)]
           [poss (total-before lengths)]
           [bins (map inst->blist ilist poss)])
      (list->bytes (map sbyte->byte (apply append bins))))))
           
            
(define assemble
  (lambda (iname oname)
    (let* ([ast (expand-file iname)]
           [flat (flatten ast)]
           [labeled-program (resolve-labels flat)]
           [pure-ilist (resolve-values labeled-program)]
           [binary (build-binary pure-ilist)])
      (display labeled-program)
      (save-binfile! oname binary))))