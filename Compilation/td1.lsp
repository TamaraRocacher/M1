(defun f(x) (+ v x))
(defun g(v) (* 5 (f (+ v 2))))
(defun fact (n)
  (if (= n 0) 1 (* n ( fact (- n 1)))))

(defun fibo (n)
  (if (= n 0)
      0
    (if (= n 1)
	1
      (+ (fibo ( - n 1)) (fibo (- n 2))))))

