## compare AUC
library("caTools")
colAUC(cbind(svm.test, nb.test), test$DESEXED, plotROC = T)

