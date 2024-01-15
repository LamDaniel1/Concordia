; Daniel Lam
; ID: 40248073
(ns menu
    (:require [clojure.string :as str]))

; (X) Helper function to write multi-line strings - 
; https://stackoverflow.com/questions/11064699/clojure-long-literal-string
(defn long-str [& strings] (clojure.string/join "\n" strings))

; not-found-str returns a string whenever input does not match with database
(def not_found_str "\nItem not found. Please try again (type 'exit' to leave).")

; print-row formats certain db row and prints to CLI
(defn print-row
  [row]
  (println (into [] (map #(str "\"" % "\"") row))))

; read-user-input prompts user for an input and returns value
(defn read-user-input
  "read-user-input is a function that asks the user for input for the menu"
  []
  (print "\n>>> ")
  (flush)
  (read-line))

; display-menu prints the SIS menu interface
(defn display-menu
  "display-menu is a function that shows the menu of options to the user"
  []
  (long-str "\n*** SIS-MENU ***"
                    "------------------" 
                    "1. Display Courses"
                    "2. Display Students"
                    "3. Display Grades"
                    "4. Display Student Record"
                    "5. Calculate GPA"
                    "6. Course Average"
                    "7. Exit"))