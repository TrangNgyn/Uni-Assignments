library(e1071)
##SVM model
##tune parameters
tune = tune.svm(DESEXED ~ .,
                data = train, 
                gamma = 10^(-5:-1),
                cost = 10^(-3:-1))
tune$best.parameters
##train the model
animals.svm <- svm(DESEXED ~ BREED_DESC + COLOUR_DES + ANIMAL_TYPE,
                   data = train,
                   gamma = 0.1, cost = 10)
summary(animals.svm)

library(caret)
##confusion matrix for train set
svm.train <- predict(animals.svm, train)
cm.svm.train = confusionMatrix(svm.train, train$DESEXED)
cm.svm.train$table
##overall accuracy
print(paste("Overall accuracy:", cm.svm.train$overall["Accuracy"]))

##predict on the test set
##the resulting confusion matrix for the test set
svm.test <- predict(animals.svm, newdata = test)
cm.svm = confusionMatrix(svm.test, test$DESEXED)
cm.svm$table
##overall accuracy
print(paste("Overall accuracy:", cm.svm$overall["Accuracy"]))

