; Daniel Lam
; ID: 40248073
(ns db
    (:require [clojure.string :as str])
)

; combine-elements-courses takes a vector [] and returns the string combination of elem 1 and 2 
(defn combine-elements-courses
  [vector]
  (str (vector 1) " " (vector 2)))

; remove-element removes a certain elem from a vector using index
(defn remove-element [vector index]
  (into (subvec vector 0 index) (subvec vector (inc index))))

; load-data loops through raw DB files, splits based "|" and then splits again based on each new line
(defn load-data
  [file]
  (into []
        (map #(into []
                (map str/trim 
                     (str/split % #"\|")))
                 (map str/trim
                      (str/split (slurp file) #"\n")))))
