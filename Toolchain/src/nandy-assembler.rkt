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
        rep)))  ; if it's not a numeric literal it's a symbol, deal with it later

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
   (list (cons "@include" (idesc @include-exp (list parse-fpath)))
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

(define macro-expand
  (lambda (exp)
    (cond [(@include-exp? exp) (resolve-file (@include-exp-path exp))]
          [(call-exp? exp) (list-exp (list (rdi-exp (bytenum (call-exp-label exp) 0))
                                           (wr-exp 'dx)
                                           (rdi-exp (bytenum (call-exp-label exp) 1))
                                           (wr-exp 'dy)
                                           (jar-exp)))]
          [(move-exp? exp) (cond [(eq? (move-exp-to exp) 'acc) (rd-exp (move-exp-from exp))]
                                 [(eq? (move-exp-from exp) 'acc) (wr-exp (move-exp-to exp))]
                                 [else (list-exp (list (sw-exp (move-exp-to exp))
                                                       (rd-exp (move-exp-from exp))
                                                       (sw-exp (move-exp-to exp))))])]
          [(list-exp? exp) (list-exp (map macro-expand (list-exp-contents exp)))]
          [(label-exp? exp) (label-exp (label-exp-label exp) (macro-expand (label-exp-target exp)))]
          [else exp])))

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

(define exp-length
  (lambda (exp)
    (cond [(rd-exp? exp) 1]
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
                    [(label-exp? (car exps)) (recurse (cons (label-exp-target (car exps)) (cdr exps))
                                                      (hash-set ltab (label-exp-label (car exps)) index)
                                                      clean-exps index)]
                    [else (recurse (cdr exps) ltab (cons (car exps) clean-exps) (+ index (exp-length (car exps))))])))
        (raise "Can only resolve labels on list expressions"))))
  

; File handling
(define load-file
  (lambda (fname)
    (string->immutable-string (port->string
                               (open-input-file fname #:mode 'text)))))
(define organize-str
  (lambda (codestr)
    (let* ([lines (string-split codestr linebrk-token)] ; break into lines
           [lines-csplit (map (lambda (l) (string-split l comment-token #:trim? #f)) lines)]
           [lines-nc (map car (filter (lambda (item) (not (null? item)))
                                      lines-csplit))] ; remove comments
           [lines-norm (filter non-empty-string? (map string-normalize-spaces lines-nc))]) ; remove whitespace
      (list-exp (map text-exp lines-norm)))))

(define resolve-file
  (lambda (fname)
    (macro-expand (parse (organize-str (load-file fname))))))

(define assemble
  (lambda (fname)
    (resolve-labels (flatten (resolve-file fname)))))