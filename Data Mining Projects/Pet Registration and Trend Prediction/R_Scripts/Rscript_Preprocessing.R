setwd("C:/Users/trang/OneDrive/Documents/UOW/Autumn 2020/INFO411_Data mining/Project")
library(dplyr)
library(ggplot2)

##read the data files
cats <- read.csv("./cats.csv")
head(cats)
dogs <- read.csv("./dogs.csv")
head(dogs)


##remove observations with missing values
dogs <- dogs[complete.cases(dogs), ]
cats <- cats[complete.cases(cats), ]
##merge the two datasets
animals <- rbind(dogs, cats)
animals <- animals %>% rename(ANIMAL_TYPE = ï..ANIMAL_TYP)

##remove outliers
suburbs <- c("BANGHOLME", "LYNDHURST", "DANDENONG SOUTH")
animals <- animals[which(!animals$SUBURB %in% suburbs),]
##there are only 7 levels of suburb factor left
animals$SUBURB <- factor(animals$SUBURB, levels = unique(animals$SUBURB))
animals$POST_CODE = factor(animals$POST_CODE)

##Shuffle the dataset for some randomnes
#shuffle the dataframe
set.seed(42)
rows <- sample(1:nrow(animals))
animals <- animals[rows,]

##split to training and test sets
sub <- sample (floor(nrow (animals) * 0.5))
train <- animals[sub,]
test <- animals[-sub,]


##Feature selection
#install.packages(c("mlbench", "FSelector"))
library(mlbench)# For data
library(FSelector)#For method

#Calculate the chi square statistics 
weights<- chi.squared(DESEXED~., train)
# Print the results 
print(weights)
# Select top five variables
subset<- cutoff.k(weights, 3)

# Print the final formula that can be used in classification
f<- as.simple.formula(subset, "DESEXED")
print(f)
