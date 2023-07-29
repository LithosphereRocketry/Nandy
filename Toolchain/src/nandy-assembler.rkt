#lang racket
(define linebrk-token "\n")
(define comment-token "#")
(define label-token ":")

; == AST expressions here ==
; Abstract constructs
(struct text-exp (str) #:transparent)
(struct list-exp (contents) #:transparent)
(struct label-exp (label target) #:transparent)

; Real instructions
(struct none-exp () #:transparent)
(struct nop-exp () #:transparent)
(struct rd-exp (reg) #:transparent)
(struct wr-exp (reg) #:transparent)
(struct sw-exp (reg) #:transparent)
(struct ja-exp () #:transparent)
(struct jar-exp () #:transparent)
(struct rdi-exp (num) #:transparent)

; Pseudoinstructions
(struct move-exp (from to) #:transparent)
(struct swap-exp (a b) #:transparent)

(define parse-reg
  (lambda (name)
    (case name
      [("acc") 'acc]
      [("x" "dx" "dl") 'dx]
      [("y" "dy" "dh") 'dy]
      [("sp") 'sp]
      [("io") 'io]
      [else (raise (string-append "Unrecognized register: " name))])))

(define parse-number
  (lambda (rep)
    (string->number rep))) ; todo: more things here

; == Text -> instruction translation here
(struct idesc (generator arg-parsers))
(define instructions
  (make-immutable-hash
   (list (cons "rd" (idesc rd-exp (list parse-reg)))
         (cons "wr" (idesc wr-exp (list parse-reg)))
         (cons "sw" (idesc sw-exp (list parse-reg)))
         (cons "ja" (idesc ja-exp '()))
         (cons "jar" (idesc jar-exp '()))
         (cons "rdi" (idesc rdi-exp (list parse-number))))))

(define parse-str
  (lambda (str)
    (let recurse ([linetok (string-split str label-token #:trim? #f)])
      (if (> (length linetok) 1)
          (label-exp (car linetok) (recurse (cdr linetok)))
          (let ([tok (map string-normalize-spaces (string-split (car linetok)))])
            (if (null? tok)
                (none-exp)
                (parse-inst (car tok) (cdr tok))))))))

; This is where instruction definitions go
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

(define load-str
  (lambda (fname)
    (string->immutable-string (port->string
                               (open-input-file fname #:mode 'text)))))

(define organize-str
  (lambda (codestr)
    (let* ([lines (string-split codestr linebrk-token)] ; break into lines
           [lines-csplit (map (lambda (l) (string-split l comment-token #:trim? #f)) lines)]
           [lines-nc (map car (filter
                               (lambda (item) (not (null? item)))
                               lines-csplit))] ; remove comments
           [lines-norm (filter non-empty-string? (map string-normalize-spaces lines-nc))]) ; remove whitespace
      (list-exp (map text-exp lines-norm)))))

(define assemble
  (lambda (fname)
    (parse (organize-str (load-str fname)))))