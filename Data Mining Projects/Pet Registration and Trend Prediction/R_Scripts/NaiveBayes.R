##naive bayes##
library(naivebayes)
animals.nb <- naive_bayes(DESEXED ~ BREED_DESC + COLOUR_DES + ANIMAL_TYPE, 
                         data = train)
summary(animals.nb)
##confusion matrix for train set
nb.train <- predict(animals.nb, train, type = "class")
cm.nb.train = confusionMatrix(nb.train, train$DESEXED)
cm.nb.train$table
##overall accuracy
print(paste("Overall accuracy:", cm.nb.train$overall["Accuracy"]))

##predict on the test set
##the resulting confusion matrix for the test set
nb.test <- predict(animals.nb, test, type = "class")
cm.nb = confusionMatrix(nb.test, test$DESEXED)
cm.nb$table
##overall accuracy
print(paste("Overall accuracy:", cm.nb$overall["Accuracy"]))

