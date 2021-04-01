library(RSNNS)
setwd("C:/Users/trang/OneDrive/Documents/UOW/Autumn 2020/INFO411_Data mining/Assignments/Ass1")
#Load dataset
fullDataSet <- read.csv("creditworthiness.csv")
fullDataSet <- data_raw[,c(1,2,3,6,9,46)]
#select all entries for which the credit rating is known
knownData <- subset(fullDataSet, fullDataSet[,6] > 0)

#select all entries for which the credit rating is unknown
unknownData <- subset(fullDataSet, fullDataSet[,6] == 0)

#separate value from targets
trainValues <- knownData[,1:5]
trainTargets <- decodeClassLabels(knownData[,6])
unknownsValues <- unknownData[,1:5]

#split dataset into traing and test set
trainset <- splitForTrainingAndTest(trainValues, trainTargets, ratio=0.5)
trainset <- normTrainingAndTestSet(trainset)

model <- mlp(trainset$inputsTrain, trainset$targetsTrain, size=5, learnFuncParams=c(0.01), maxit=250, inputsTest=trainset$inputsTest, targetsTest=trainset$targetsTest)
predictTestSet <- predict(model,trainset$inputsTest)

confusionMatrix(trainset$targetsTrain,fitted.values(model))
confusionMatrix(trainset$targetsTest,predictTestSet)

par(mfrow=c(2,2))
plotIterativeError(model)
plotRegressionError(predictTestSet[,2], trainset$targetsTest[,2])
plotROC(fitted.values(model)[,2], trainset$targetsTrain[,2])
plotROC(predictTestSet[,2], trainset$targetsTest[,2])

#confusion matrix with 402040-method
confusionMatrix(trainset$targetsTrain, encodeClassLabels(fitted.values(model),method="402040", l=0.4, h=0.6))


#show detailed information of the model
summary(model)
model
weightMatrix(model)
extractNetInfo(model)

