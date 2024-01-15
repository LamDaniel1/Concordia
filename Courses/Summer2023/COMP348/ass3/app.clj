; Daniel Lam
; ID: 40248073
(ns app
  (:require [db])
  (:require [menu])
  (:require [clojure.string :as str]))

; Initialize variables for program
(def has-not-exit (atom true))
(def user-input (atom nil))
(def studDB (db/load-data "studs.txt"))
(def courseDB (db/load-data "courses.txt"))
(def gradeDB (db/load-data "grades.txt"))
(def grades-map
  {"A+" 4.3, "A" 4, "A-" 3.7,
   "B+" 3.3, "B" 3, "B-" 2.7,
   "C+" 2.3, "C" 2, "C-" 1.7,
   "D+" 1.3, "D" 1, "D-" 0.7,
   "F" 0
   })

; display-courses-option is a function that prints all the information of each course
(defn display-courses-option
  "User has chosen display courses option"
  []
  
  ; Intro print
  (println "\nPrinting all records of courses...\n")

  (doseq [row courseDB]
    ; Store full course name and number into a variable (e.g "COMP 348")
    (let [full_course_name (db/combine-elements-courses row)]
      
      ; Store row created into a variable with second element removed (e.g. ["1" "348" "3" "Principles of Programming Languages"])
      (let [temp_row (into [] (db/remove-element row 1))]

        ; Replace second element of temp_row with new full course name to get desired result
        ; e.g. ["1" "COMP348" "3" "Principles of Programming Languages"]
        (menu/print-row (assoc temp_row 1 full_course_name))))))
  
; display-student-option is a function that prints all the information of each course
(defn display-students-option
  "User has chosen display students option"
  []

  ; Intro Print
  (println "\nPrinting all records of students...\n")

  ; Loop through each row of studDB and print info
  (doseq [row studDB]
    (menu/print-row row)))

; display-grades-option is a function that prints all the information of each grade
(defn display-grades-option
  "User has chosen display grades option"
  []

  ; Intro Print
  (println "\nPrinting all records of grades...\n")

  ; Loop through each row of gradeDB and print info
  (doseq [row gradeDB]
    (menu/print-row row)))

; display-student-record-option takes in student ID and prints all grades associated with student for each course
(defn display-student-record-option
  "User has chosen display student record option"
  []
  
  ; Intro Prints
  (println "\nDisplay Student Record chosen...")
  (println "\nPlease enter the student ID that you are searching for:")

  ; Loop user input CLI while student ID not found
  (let [has_not_found (atom true)]
    (while (true? @has_not_found)
      
      ; user-input
      (let [stud_ID_input (menu/read-user-input)]
        (reset! user-input stud_ID_input)

        ; if the user did not input exit, check if student id exists
        (if (not= (str/lower-case stud_ID_input) "exit")
          (do
            (doseq [studDB_row studDB]
              
              ; if student ID is found inside database
              (if (= (first studDB_row) stud_ID_input)
                (do

                  ; stop loop and print student name
                  (reset! has_not_found false)
                  (println (str "\nStudent ID \"" stud_ID_input "\" has been found. Displaying student records...\n"))
                  (menu/print-row (vector (first studDB_row) (studDB_row 1)))
                  
                  ; query through database to all relevant information and print it
                  (doseq [gradeDB_row gradeDB]
                    (if (= (first gradeDB_row) stud_ID_input)
                      (let [course_id (gradeDB_row 1)]
                        (let [grade_info (subvec gradeDB_row 2)]
                          (doseq [courseDB_row courseDB]
                            (if (= (first courseDB_row) course_id)
                              (let [full_course_name (db/combine-elements-courses courseDB_row)]
                                (menu/print-row (into [] (concat (vector full_course_name (courseDB_row 4)) grade_info)))))))))))))
            
            (if (= @has_not_found true)
              (println menu/not_found_str)))
          
          (do
            (reset! has_not_found false)))))
          ))

; convert-grade-to-gpa is a helper function that takes a grade letter and returns gpa associated
(defn convert-grade-to-gpa
  [grade_letter]
  (get grades-map grade_letter)
  )

; calculate-gpa-option asks the user for a student ID and prints GPA associated with user
(defn calculate-gpa-option
  "User has chosen calculate gpa option"
  []
  (println "\nCalculate GPA chosen...")
  (println "\nPlease enter the student ID that you are searching for:")

  ; loop while student ID is not found
  (let [has_not_found (atom true)]
    (while (true? @has_not_found)
      
      ; user input
      (let [stud_ID_input (menu/read-user-input)]
        (reset! user-input stud_ID_input)

        ; if user has not inputted exit
        (if (not= (str/lower-case stud_ID_input) "exit")
          (do
            
            ; loop through student database
            (doseq [studDB_row studDB]
              (if (= (first studDB_row) stud_ID_input)
                (do
                  
                  ; if student ID is found, stop loop and print student name
                  (reset! has_not_found false)
                  (println (str "\nStudent ID \"" stud_ID_input "\" has been found. Displaying student records...\n"))
                  (menu/print-row (vector (first studDB_row) (studDB_row 1)))

                  ; query through database to find all relevant info
                  (let [total-gpa (atom 0)]
                    (let [total-credits (atom 0)]
                      
                      ; loop through grade database
                      (doseq [gradeDB_row gradeDB]
                        (if (= (first gradeDB_row) stud_ID_input)
                          
                          ; if student id matches, take letter grade earned and convert to GPA
                          (let [gpa-grade (atom (convert-grade-to-gpa (gradeDB_row 3)))]
                            (let [courseID (atom (gradeDB_row 1))]
                              (doseq [courseDB_row courseDB]

                                (if (= (first courseDB_row) @courseID)
                                  (do 
                                    
                                    ; store credits earned from certain course
                                    (let [credits (atom (read-string (courseDB_row 3)))]
                                      
                                      ; sum up total gpa based on credits * gpa
                                      (reset! total-gpa (+ @total-gpa (* @credits @gpa-grade)))

                                      ; sum up total-credits of all courses
                                      (reset! total-credits (+ @total-credits @credits))
                                      ))))))))
                      
                      ; format average GPA and print
                      (println (str "GPA Average: " (format "%.2f" (/ @total-gpa @total-credits))))
                      ))
                  ))
              )

            (if (= @has_not_found true)
              (println menu/not_found_str)))
            (do
              (reset! has_not_found false))
            )))
          ))

; course-average-option takes the full course name and prints its gpa average
(defn course-average-option
  "User has chosen course average option"
  []

  ; Intro Prints
  (println "\nDisplay course average chosen...")
  (println "\nPlease enter the full course name that you are searching for (e.g 'COMP 232'):")

  ; loop while course name is not found
  (let [has_not_found (atom true)]
    (while (true? @has_not_found)
      
      ; user-input
      (let [course_input (atom (menu/read-user-input))]
        (reset! user-input course_input)

        ; if user has not inputted exit
        (if (not= (str/lower-case @course_input) "exit")
          (do
            
            ; format input back to uppercase to match DB
            (reset! course_input (str/upper-case @course_input))

            ; loop through course DB to match course name
            (doseq [courseDB_row courseDB]
              (let [full_course_name (db/combine-elements-courses courseDB_row)] 

              (if (= full_course_name @course_input)
                (do
                  
                  ; course name matched, stop loop and store course id
                  (reset! has_not_found false)
                  (println (str "\nCourse \"" full_course_name "\" has been found. Displaying course records...\n"))
                  (let [course-id (atom (first courseDB_row))]
                    
                    ; store total number of courses, total grade and semester name
                    (let [count (atom 0)]
                      (let [total-grade (atom 0)]
                        (let [semester (atom nil)]
                          (doseq [grade-row gradeDB]
                            (reset! semester (grade-row 2))

                            ; if course-id matches corresponding grade
                            (if (= (grade-row 1) @course-id)
                              (let [gpa-grade (atom (convert-grade-to-gpa (grade-row 3)))]
                                
                                ; add gpa grade from course to total gpa of course
                                (reset! total-grade (+ @total-grade @gpa-grade))

                                ; increment number of courses by 1
                                (reset! count (+ @count 1))
                                )))
                          
                          ; if course has been found in grade DB
                          (if (= @count 0)
                            #_{:clj-kondo/ignore [:redundant-do]}
                            (do (println "No grades found for '" full_course_name "'."))
                            #_{:clj-kondo/ignore [:redundant-do]}

                            ; print full info on course + grade average
                            (do 
                              (menu/print-row (vector full_course_name @semester (format "%.2f" (/ @total-grade @count))))))
                          ))
                        
                        ))))
                ))
                (if (= @has_not_found true)
                  (println menu/not_found_str)))
              (do
                (reset! has_not_found false))
              )
            
))))

; exit-option stops menu loop
(defn exit-option
  "User has chosen exit option"
  []
  (reset! has-not-exit false))

; process-input is a function that takes user-input and chooses correct option
(defn process-input 
  [input]
  (let [clean-input (atom (str/lower-case @input))]
    (cond
      ; 1. Display Courses
      (= @clean-input "1") (display-courses-option)
      (= @clean-input "display courses") (display-courses-option)

      ; 2. Display Students
      (= @clean-input "2") (display-students-option)
      (= @clean-input "display students") (display-students-option)

      ; 3. Display Grades
      (= @clean-input "3") (display-grades-option)
      (= @clean-input "display grades") (display-grades-option)

      ; 4. Display Student Record
      (= @clean-input "4") (display-student-record-option)
      (= @clean-input "display student record") (display-student-record-option)

      ; 5. Calculate GPA
      (= @clean-input "5") (calculate-gpa-option)
      (= @clean-input "calculate gpa") (calculate-gpa-option)

      ; 6. Course Average
      (= @clean-input "6") (course-average-option)
      (= @clean-input "course average") (course-average-option)

      ; 7. Exit
      (= @clean-input "7") (exit-option)
      (= @clean-input "exit") (exit-option)

      ; Option not part of menu
      :else (println "\nNot a valid option! Please try again."))))

; start runs main functionality of application
(defn start
  "Main app of program"
  []
  
  ; Display intro to user
  (println "\nWelcome to the SIS Database! Please select one of following options (either number or full text):")

  ; Continue displaying SIS menu to user while he has not exited
  (while @has-not-exit
    ; Helper function to print SIS menu and all available options
    (println (menu/display-menu))

    ; Read user input
    (let [input (menu/read-user-input)]
      (reset! user-input input)
      (process-input user-input))

    ; Check if user has chosen exit, if yes then stop loop and print exit message
    (when (not @has-not-exit)
      (println "\nThank you for using the SIS Database. See you again soon!\n"))))

; Run program by invoking start function
(start)