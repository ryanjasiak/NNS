#' LegacyNNS Stack
#'
#' Prediction model using the predictions of the LegacyNNS base models \link{LegacyNNS.reg} as features (i.e. meta-features) for the stacked model.
#'
#' @param IVs.train a vector, matrix or data frame of variables of numeric or factor data types.
#' @param DV.train a numeric or factor vector with compatible dimensions to \code{(IVs.train)}.
#' @param IVs.test a vector, matrix or data frame of variables of numeric or factor data types with compatible dimensions to \code{(IVs.train)}.  If NULL, will use \code{(IVs.train)} as default.
#' @param type \code{NULL} (default).  To perform a classification of discrete integer classes from factor target variable \code{(DV.train)} with a base category of 1, set to \code{(type = "CLASS")}, else for continuous \code{(DV.train)} set to \code{(type = NULL)}.   Like a logistic regression, this setting is not necessary for target variable of two classes e.g. [0, 1].
#' @param obj.fn expression; \code{expression(sum((predicted - actual)^2))} (default) Sum of squared errors is the default objective function.  Any \code{expression()} using the specific terms \code{predicted} and \code{actual} can be used.
#' @param objective options: ("min", "max") \code{"min"} (default) Select whether to minimize or maximize the objective function \code{obj.fn}.
#' @param optimize.threshold logical; \code{TRUE} (default) Will optimize the probability threshold value for rounding in classification problems.  If \code{FALSE}, returns 0.5.
#' @param dist options:("L1", "L2", "DTW", "FACTOR") the method of distance calculation; Selects the distance calculation used. \code{dist = "L2"} (default) selects the Euclidean distance and \code{(dist = "L1")} selects the Manhattan distance; \code{(dist = "DTW")} selects the dynamic time warping distance; \code{(dist = "FACTOR")} uses a frequency.
#' @param CV.size numeric [0, 1]; \code{NULL} (default) Sets the cross-validation size if \code{(IVs.test = NULL)}.  Defaults to a random value between 0.2 and 0.33 for a random sampling of the training set.
#' @param balance logical; \code{FALSE} (default) Uses both up and down sampling to balance the classes.  \code{type="CLASS"} required.
#' @param ts.test integer; NULL (default) Sets the length of the test set for time-series data; typically \code{2*h} parameter value from \link{LegacyNNS.ARMA} or double known periods to forecast.
#' @param folds integer; \code{folds = 5} (default) Select the number of cross-validation folds.
#' @param order options: (integer, "max", NULL); \code{NULL} (default) Sets the order for \link{LegacyNNS.reg}, where \code{(order = "max")} is the k-nearest neighbors equivalent, which is suggested for mixed continuous and discrete (unordered, ordered) data.
#' @param norm options: ("std", "LegacyNNS", NULL); \code{NULL} (default) 3 settings offered: \code{NULL}, \code{"std"}, and \code{"LegacyNNS"}.  Selects the \code{norm} parameter in \link{LegacyNNS.reg}.
#' @param method numeric options: (1, 2); Select the LegacyNNS method to include in stack.  \code{(method = 1)} selects \link{LegacyNNS.reg}; \code{(method = 2)} selects \link{LegacyNNS.reg} dimension reduction regression.  Defaults to \code{method = c(1, 2)}, which will reduce the dimension first, then find the optimal \code{n.best}.
#' @param stack logical; \code{TRUE} (default) Uses dimension reduction output in \code{n.best} optimization, otherwise performs both analyses independently.
#' @param dim.red.method options: ("cor", "LegacyNNS.dep", "LegacyNNS.caus", "equal", "all") method for determining synthetic X* coefficients.  \code{(dim.red.method = "cor")} uses standard linear correlation for weights.  \code{(dim.red.method = "LegacyNNS.dep")} (default) uses \link{LegacyNNS.dep} for nonlinear dependence weights, while \code{(dim.red.method = "LegacyNNS.caus")} uses \link{LegacyNNS.caus} for causal weights.  \code{(dim.red.method = "all")} averages all methods for further feature engineering.
#' @param pred.int numeric [0,1]; \code{NULL} (default) Returns the associated prediction intervals with each \code{method}.
#' @param status logical; \code{TRUE} (default) Prints status update message in console.
#' @param ncores integer; value specifying the number of cores to be used in the parallelized subroutine \link{LegacyNNS.reg}. If NULL (default), the number of cores to be used is equal to the number of cores of the machine - 1.
#'
#' @return Returns a vector of fitted values for the dependent variable test set for all models.
#' \itemize{
#' \item{\code{"LegacyNNS.reg.n.best"}} returns the optimum \code{"n.best"} parameter for the \link{LegacyNNS.reg} multivariate regression.  \code{"SSE.reg"} returns the SSE for the \link{LegacyNNS.reg} multivariate regression.
#' \item{\code{"OBJfn.reg"}} returns the \code{obj.fn} for the \link{LegacyNNS.reg} regression.
#' \item{\code{"LegacyNNS.dim.red.threshold"}} returns the optimum \code{"threshold"} from the \link{LegacyNNS.reg} dimension reduction regression.
#' \item{\code{"OBJfn.dim.red"}} returns the \code{obj.fn} for the \link{LegacyNNS.reg} dimension reduction regression.
#' \item{\code{"probability.threshold"}} returns the optimum probability threshold for classification, else 0.5 when set to \code{FALSE}.
#' \item{\code{"reg"}} returns \link{LegacyNNS.reg} output.
#' \item{\code{"reg.pred.int"}} returns the prediction intervals for the regression output.
#' \item{\code{"dim.red"}} returns \link{LegacyNNS.reg} dimension reduction regression output.
#' \item{\code{"dim.red.pred.int"}} returns the prediction intervals for the dimension reduction regression output.
#' \item{\code{"stack"}} returns the output of the stacked model.
#' \item{\code{"pred.int"}} returns the prediction intervals for the stacked model.
#' }
#'
#' @author Fred Viole, OVVO Financial Systems
#' @references Viole, F. (2016) "Classification Using LegacyNNS Clustering Analysis"  \doi{10.2139/ssrn.2864711}
#'
#' @note
#' \itemize{
#' \item Incorporate any objective function from external packages (such as \code{Metrics::mape}) via \code{LegacyNNS.stack(..., obj.fn = expression(Metrics::mape(actual, predicted)), objective = "min")}
#' 
#' \item Like a logistic regression, the \code{(type = "CLASS")} setting is not necessary for target variable of two classes e.g. [0, 1].  The response variable base category should be 1 for multiple class problems.
#'
#' \item Missing data should be handled prior as well using \link{na.omit} or \link{complete.cases} on the full dataset.
#' }
#'
#' If error received:
#'
#' \code{"Error in is.data.frame(x) : object 'RP' not found"}
#'
#' reduce the \code{CV.size}.
#'
#'
#' @examples
#'  ## Using 'iris' dataset where test set [IVs.test] is 'iris' rows 141:150.
#'  \dontrun{
#'  LegacyNNS.stack(iris[1:140, 1:4], iris[1:140, 5], IVs.test = iris[141:150, 1:4], type = "CLASS")
#'
#'  ## Using 'iris' dataset to determine [n.best] and [threshold] with no test set.
#'  LegacyNNS.stack(iris[ , 1:4], iris[ , 5], type = "CLASS")
#'
#'  ## Selecting LegacyNNS.reg and dimension reduction techniques.
#'  LegacyNNS.stack(iris[1:140, 1:4], iris[1:140, 5], iris[141:150, 1:4], method = c(1, 2), type = "CLASS")
#'  }
#' @export

LegacyNNS.stack <- function(IVs.train,
                      DV.train,
                      IVs.test = NULL,
                      type = NULL,
                      obj.fn = expression( sum((predicted - actual)^2) ),
                      objective = "min",
                      optimize.threshold = TRUE,
                      dist = "L2",
                      CV.size = NULL,
                      balance = FALSE,
                      ts.test = NULL,
                      folds = 5,
                      order = NULL,
                      norm = NULL,
                      method = c(1, 2),
                      stack = TRUE,
                      dim.red.method = "cor",
                      pred.int = NULL,
                      status = TRUE,
                      ncores = NULL){
  
  if(sum(is.na(cbind(IVs.train,DV.train))) > 0) stop("You have some missing values, please address.")
  
  if(is.null(obj.fn)) stop("Please provide an objective function")
  
  if(balance && is.null(type)) warning("type = 'CLASS' selected due to balance = TRUE.")
  if(balance) type <- "CLASS"
  
  if(!is.null(type) && min(as.numeric(DV.train))==0) warning("Base response variable category should be 1, not 0.")
  
  if(any(class(IVs.train)%in%c("tbl","data.table"))) IVs.train <- as.data.frame(IVs.train)
  if(any(class(DV.train)%in%c("tbl","data.table"))) DV.train <- as.vector(unlist(DV.train))
  
  if(is.vector(IVs.train) || is.null(dim(IVs.train)) || ncol(IVs.train)==1){
    IVs.train <- data.frame(IVs.train)
    method <- 1
    order <- NULL
  }
  
  if(!is.null(type)){
    type <- tolower(type)
    if(type == "class" && identical(obj.fn,expression( sum((predicted - actual)^2) ))){
      obj.fn <- expression(mean( predicted == as.numeric(actual)))
      objective <- "max"
    }
  }
  
  objective <- tolower(objective)
  
  if(!is.null(type) && type=="class") DV.train <- as.numeric(factor(DV.train)) else DV.train <- as.numeric(DV.train)
  
  n <- ncol(IVs.train)
  
  l <- floor(sqrt(length(IVs.train[ , 1])))
  
  if(is.null(IVs.test)){
    IVs.test <- IVs.train
  } else {
    if(any(class(IVs.test)%in%c("tbl","data.table"))) IVs.test <- as.data.frame(IVs.test)
  }
  
  if(is.null(dim(IVs.test))) IVs.test <- data.frame(t(IVs.test)) else IVs.test <- data.frame(IVs.test)
  
  dist <- tolower(dist)
  
  i_s <- numeric()
  THRESHOLDS <- vector(mode = "list", folds)
  best.k <- vector(mode = "list", folds)
  best.LegacyNNS.cv <- vector(mode = "list", folds)
  best.LegacyNNS.ord <- vector(mode = "list", folds)
  
  if(is.null(colnames(IVs.train))){
    colnames.list <- lapply(1 : dim(IVs.train)[2], function(i) paste0("X", i))
    colnames(IVs.test) <- colnames(IVs.train) <- as.character(colnames.list)
  }
  
  if(2 %in% method && dim(IVs.train)[2]>1){
    if(dim.red.method=="cor"){
      var.cutoffs_1 <- abs(round(cor(data.matrix(cbind(DV.train, IVs.train)), method = "spearman")[-1,1], digits = 2))
    } else {
      var.cutoffs_1 <- abs(round(suppressWarnings(LegacyNNS.reg(IVs.train, DV.train, dim.red.method = dim.red.method, plot = FALSE, residual.plot = FALSE, order = order, ncores = ncores,
                                                          type = type, point.only = TRUE)$equation$Coefficient[-(n+1)]), digits = 2))
    }
  }
  
  if(is.null(CV.size)) new.CV.size <- round(runif(1, .2, 1/3), 3) else new.CV.size <- CV.size
  
  for(b in 1 : folds){
    if(status) message("Folds Remaining = " , folds-b," ","\r",appendLF=TRUE)
    
    set.seed(123 * b)
    
    test.set <- as.integer(seq(b, length(unlist(IVs.train[ , 1])), length.out = as.integer(new.CV.size * length(unlist(IVs.train[ , 1])))))
    
    if(!is.null(ts.test)){
      test.set <- 1:(length(DV.train) - ts.test)
    }
    
    test.set <- unlist(test.set)
    
    CV.IVs.train <- data.frame(IVs.train[c(-test.set), ])
    
    if(dim(CV.IVs.train)[2]!=dim(IVs.train)[2]) CV.IVs.train <- t(CV.IVs.train)
    if(dim(CV.IVs.train)[2]!=dim(IVs.train)[2]) CV.IVs.train <- t(CV.IVs.train)
    
    
    CV.IVs.test <- data.frame(IVs.train[test.set, ])
    if(dim(CV.IVs.test)[2]!=dim(IVs.train)[2]) CV.IVs.test <- t(CV.IVs.test)
    if(dim(CV.IVs.test)[2]!=dim(IVs.train)[2]) CV.IVs.test <- t(CV.IVs.test)
    
    CV.DV.train <- DV.train[c(-test.set)]
    CV.DV.test <- DV.train[c(test.set)]
    
    training <- cbind(IVs.train[c(-test.set),], DV.train[c(-test.set)])
    training <- training[complete.cases(training),]
    
    if(balance){
      DV.train <- as.numeric(as.factor(DV.train))

      CV.DV.train <- DV.train[c(-test.set)]
      CV.DV.test <- DV.train[c(test.set)]
      
      y_train <- as.factor(CV.DV.train)
      training_1 <- do.call(cbind, downSample(CV.IVs.train, y_train, list = TRUE))
      training_2 <- do.call(cbind, upSample(CV.IVs.train, y_train, list = TRUE))
      
      training <- rbind.data.frame(training_1, training_2)
      
      colnames(training) <- c(colnames(CV.IVs.train), names(CV.DV.train))
    }
    
    
    CV.IVs.train <- data.frame(training[, -(ncol(training))])
    
    CV.DV.train <- as.numeric(as.character(training[, ncol(training)]))
    
    
    # Dimension Reduction Regression Output
    if(2 %in% method && ncol(IVs.train)>1){
      actual <- CV.DV.test
      
      if(dim.red.method=="cor"){
        var.cutoffs_2 <- abs(round(suppressWarnings(cor(data.matrix(cbind(CV.DV.train, CV.IVs.train)), method = "spearman"))[-1,1], digits = 2))
      } else {
        var.cutoffs_2 <- abs(round(suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, dim.red.method = dim.red.method, plot = FALSE, residual.plot = FALSE, order = order, ncores = ncores,
                                                            type = type, point.only = TRUE)$equation$Coefficient[-(n+1)]), digits = 2))
      }
      
      var.cutoffs <- c(pmin(var.cutoffs_1, (pmax(var.cutoffs_1, var.cutoffs_2) + pmin(var.cutoffs_1, var.cutoffs_2))/2))
      
      var.cutoffs <- var.cutoffs[var.cutoffs < 1 & var.cutoffs >= 0]
      
      var.cutoffs[is.na(var.cutoffs)] <- 0
      
      var.cutoffs <- rev(sort(unique(var.cutoffs)))[-1]
      
      if(is.null(var.cutoffs)) var.cutoffs <- 0
      
      if(n == 2) var.cutoffs <- c(var.cutoffs, 0)
      
      if(dist=="factor") var.cutoffs <- var.cutoffs[-1]
      if(dim.red.method=="equal") var.cutoffs <- 0
      
      threshold_results_2 <- vector(mode = "list", length = length(var.cutoffs))
      LegacyNNS.ord <- numeric(length(var.cutoffs))
      
      for(i in 1:length(var.cutoffs)){
        if(status){
          message("Current LegacyNNS.reg(... , threshold = ", var.cutoffs[i] ," ) MAX Iterations Remaining = " , length(var.cutoffs)-i," ","\r",appendLF=TRUE)
        }
        
        predicted <- suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, point.est = CV.IVs.test, plot = FALSE, dim.red.method = dim.red.method, threshold = var.cutoffs[i], order = order, ncores = ncores,
                                              type = NULL, dist = dist, point.only = TRUE)$Point.est)
        
        predicted[is.na(predicted)] <- gravity(na.omit(predicted))
        
        if(!is.null(type)){
          pred_matrix <- sapply(seq(.01, .99, .01), function(z) ifelse(predicted%%1<z, as.integer(floor(predicted)), as.integer(ceiling(predicted))))
          z <- apply(pred_matrix, 2, function(z) mean(z == as.numeric(actual)))
          threshold_results_2[[i]] <- seq(.01,.99, .01)[as.integer(median(which(z==max(z))))]
          
          predicted <- ifelse(predicted%%1 < threshold_results_2[[i]], floor(predicted), ceiling(predicted))
        }
        
        LegacyNNS.ord[i] <- eval(obj.fn)
       
        i_s[i] <- i

        best.threshold <- ifelse(length(i_s <=2), var.cutoffs[1], var.cutoffs[mode_class(i_s) - 1])
        THRESHOLDS[[b]] <- best.threshold

        if(objective=="min"){
          best.LegacyNNS.ord[[b]] <- min(na.omit(LegacyNNS.ord))
          if(is.na(LegacyNNS.ord[1])) LegacyNNS.ord[1] <- Inf
        } else {
          best.LegacyNNS.ord[[b]] <- max(na.omit(LegacyNNS.ord))
          if(is.na(LegacyNNS.ord[1])) LegacyNNS.ord[1] <- -Inf
        }
        
        if(i > 2 && is.na(LegacyNNS.ord[i])) break
        if(i > 2 && (LegacyNNS.ord[i] >= LegacyNNS.ord[i-1]) && (LegacyNNS.ord[i] >= LegacyNNS.ord[i-2])) break
      }
      
      
      relevant_vars <- colnames(IVs.train)
      if(is.null(relevant_vars)) relevant_vars <- 1:n
    
      if(b==folds){
        threshold.table <- sort(table(unlist(THRESHOLDS)), decreasing = TRUE)
        
        LegacyNNS.ord.threshold <- gravity(as.numeric(names(threshold.table[threshold.table==max(threshold.table)])))
        if(is.na(LegacyNNS.ord.threshold)) LegacyNNS.ord.threshold <- 0

        LegacyNNS.method.2 <- (LegacyNNS.reg(IVs.train, DV.train, point.est = IVs.test, dim.red.method = dim.red.method, plot = FALSE, order = order, threshold = LegacyNNS.ord.threshold, ncores = ncores,
                                                 type = NULL, point.only = TRUE, confidence.interval = pred.int))
  
        actual <- LegacyNNS.method.2$Fitted.xy$y
        predicted <- LegacyNNS.method.2$Fitted.xy$y.hat
        pred.int.2 <- LegacyNNS.method.2$pred.int
        
        best.LegacyNNS.ord <- eval(obj.fn)
        
        rel_vars <- LegacyNNS.method.2$equation
        
        rel_vars <- which(rel_vars$Coefficient>0)
        rel_vars <- rel_vars[rel_vars <= n]
        
        if(is.null(rel_vars) || length(rel_vars)==0) rel_vars <- 1:n
        
        if(!stack) relevant_vars <- 1:n else relevant_vars <- rel_vars
        
        if(all(relevant_vars=="FALSE")) relevant_vars <- 1:n

        
        if(!is.null(type) && !is.null(LegacyNNS.method.2$Point.est)){
          threshold_results_2 <- mean(unlist(threshold_results_2))
          
          LegacyNNS.method.2 <- ifelse(LegacyNNS.method.2$Point.est%%1 < threshold_results_2, floor(LegacyNNS.method.2$Point.est), ceiling(LegacyNNS.method.2$Point.est))
          LegacyNNS.method.2 <- pmin(LegacyNNS.method.2, max(as.numeric(DV.train)))
          LegacyNNS.method.2 <- pmax(LegacyNNS.method.2, min(as.numeric(DV.train)))
        } else {
          LegacyNNS.method.2 <- LegacyNNS.method.2$Point.est
        }
        
        
      }
      
    } else {
      THRESHOLDS <- NA
      test.set.2 <- NULL
      LegacyNNS.method.2 <- NA
      if(objective=='min'){best.LegacyNNS.ord <- Inf} else {best.LegacyNNS.ord <- -Inf}
      LegacyNNS.ord.threshold <- NA
      threshold_results_2 <- NA
      relevant_vars <- 1:n
    } # 2 %in% method
    
    
      
    if(1 %in% method){
      actual <- CV.DV.test
      
      if(is.character(relevant_vars)) relevant_vars <- relevant_vars!=""
      
      if(is.logical(relevant_vars)){
        CV.IVs.train <- data.frame(CV.IVs.train[, relevant_vars])
        CV.IVs.test <- data.frame(CV.IVs.test[, relevant_vars])
      }
      
      
      if(dim(CV.IVs.train)[2]!=n) CV.IVs.train <- t(CV.IVs.train)
      if(dim(CV.IVs.train)[2]!=n) CV.IVs.train <- t(CV.IVs.train)
      
      if(dim(CV.IVs.test)[2]!=n) CV.IVs.test <- t(CV.IVs.test)
      if(dim(CV.IVs.test)[2]!=n) CV.IVs.test <- t(CV.IVs.test)
      
      threshold_results_1 <- vector(mode = "list", length(c(1:l, length(IVs.train[ , 1]))))
      LegacyNNS.cv.1 <- numeric()
      
      q <- length(IVs.train[ , 1])
      
      for(i in c(1:l, q)){
        index <- which(c(1:l, q) == i)
        if(status){
          message("Current LegacyNNS.reg(... , n.best = ", i ," ) MAX Iterations Remaining = " ,l-index+1," ","\r",appendLF=TRUE)
        }
        
        if(index==1){
          setup <- suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, point.est = CV.IVs.test, plot = FALSE, residual.plot = FALSE, n.best = 1, order = order,
                                            type = type, factor.2.dummy = TRUE, dist = dist, ncores = ncores, point.only = FALSE))
          
          if(is.null(dim(setup$RPM))) setup$RPM <- setup$regression.points
          
          if(is.null(dim(setup$RPM))  && is.null(setup$regression.points)){
            setup <- suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, point.est = CV.IVs.test, plot = FALSE, residual.plot = FALSE, n.best = 1, order = "max",
                                              type = type, factor.2.dummy = TRUE, dist = dist, ncores = ncores, point.only = FALSE))
          }
          
          if(is.null(dim(setup$RPM))) setup$RPM <- setup$regression.points
          
          LegacyNNS.id <- setup$Fitted.xy$LegacyNNS.ID
          original.DV <- setup$Fitted.xy$y
         
          predicted <- setup$Point.est
         
          predicted[is.na(predicted)] <- mean(predicted, na.rm = TRUE)
          if(length(unique(predicted))==1){
            pred_matrix <- matrix(replicate(100, predicted), nrow = length(predicted))
          } else {
            pred_matrix <- sapply(seq(.01, .99, .01), function(z) ifelse(predicted%%1<z, as.integer(floor(predicted)), as.integer(ceiling(predicted))))
          }
            
          threshold_results_1[index] <- seq(.01,.99, .01)[which.max(apply(pred_matrix, 2, function(z) mean(z == as.numeric(actual))))]

          predicted <- ifelse(predicted%%1 < threshold_results_1[index], floor(predicted), ceiling(predicted))
          
        } else {
          
          
          if(!is.null(dim(CV.IVs.train))){
            if(ncol(CV.IVs.train)>1){
              CV.IVs.test.new <- data.table::data.table(apply(data.frame(CV.IVs.test), 2, function(z) factor_2_dummy_FR(z)))
              
              CV.IVs.test.new <- CV.IVs.test.new[, DISTANCES :=  LegacyNNS.distance(rpm = setup$RPM, dist.estimate = .SD, k = i, class = type)[1], by = 1:nrow(CV.IVs.test)]
              
              predicted <- as.numeric(unlist(CV.IVs.test.new$DISTANCES))
              rm(CV.IVs.test.new)
            } else {
              predicted <-  suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, point.est = CV.IVs.test, plot = FALSE, residual.plot = FALSE, n.best = i, order = order, ncores = ncores,
                                                     type = type, factor.2.dummy = TRUE, dist = dist, point.only = TRUE)$Point.est)
            }
          } else {
            predicted <-  suppressWarnings(LegacyNNS.reg(CV.IVs.train, CV.DV.train, point.est = unlist(CV.IVs.test), plot = FALSE, residual.plot = FALSE, n.best = i, order = order, ncores = ncores,
                                                   type = type, factor.2.dummy = TRUE, dist = dist, point.only = TRUE)$Point.est)
          }
          
          if(!is.null(type)){
            if(length(unique(predicted))==1){
              pred_matrix <- matrix(replicate(100, predicted), nrow = length(predicted))
            } else {
              pred_matrix <- sapply(seq(.01, .99, .01), function(z) ifelse(predicted%%1<z, as.integer(floor(predicted)), as.integer(ceiling(predicted))))
            }
            
            z <- apply(pred_matrix, 2, function(z) mean(z == as.numeric(actual)))
            threshold_results_1[[index]] <- seq(.01,.99, .01)[as.integer(median(which(z==max(z))))]
            
            predicted <- ifelse(predicted%%1 < threshold_results_1[[index]], floor(predicted), ceiling(predicted))
          }
          
          
        }
        
        LegacyNNS.cv.1[index] <- eval(obj.fn)
        
        if(length(na.omit(LegacyNNS.cv.1)) > 3){
          if(objective=="min") LegacyNNS.cv.1[is.na(LegacyNNS.cv.1)] <- max(na.omit(LegacyNNS.cv.1)) else LegacyNNS.cv.1[is.na(LegacyNNS.cv.1)] <- min(na.omit(LegacyNNS.cv.1))
          if(objective=='min' && LegacyNNS.cv.1[index]>=LegacyNNS.cv.1[index-1] && LegacyNNS.cv.1[index]>=LegacyNNS.cv.1[index-2]){ break }
          if(objective=='max' && LegacyNNS.cv.1[index]<=LegacyNNS.cv.1[index-1] && LegacyNNS.cv.1[index]<=LegacyNNS.cv.1[index-2]){ break }
        }
      }
      
      
      ks <- c(1:l, q)[!is.na(LegacyNNS.cv.1)]
      
      if(objective=='min'){
        k <- ks[which.min(na.omit(LegacyNNS.cv.1))]
        LegacyNNS.cv.1 <- min(na.omit(LegacyNNS.cv.1))
      } else {
        k <- ks[which.max(na.omit(LegacyNNS.cv.1))]
        LegacyNNS.cv.1 <- max(na.omit(LegacyNNS.cv.1))
      }
      
      
      best.k[[b]] <- k
      best.LegacyNNS.cv[[b]] <- if(!is.null(type)) min(max(LegacyNNS.cv.1,0),1) else LegacyNNS.cv.1
      
      if(b==folds){
        ks <- table(unlist(best.k))
        
        best.k <-  mode_class(as.numeric(rep(names(ks), as.numeric(unlist(ks)))))

        if(length(relevant_vars)>1){
            LegacyNNS.method.1 <- suppressWarnings(LegacyNNS.reg(IVs.train[ , relevant_vars], DV.train, point.est = IVs.test[, relevant_vars], plot = FALSE, n.best = best.k, order = order, ncores = ncores,
                                                     type = NULL, point.only = FALSE, confidence.interval = pred.int))
        } else {
            LegacyNNS.method.1 <- suppressWarnings(LegacyNNS.reg(IVs.train[ , relevant_vars], DV.train, point.est = unlist(IVs.test[, relevant_vars]), plot = FALSE, n.best = best.k, order = order, ncores = ncores,
                                                    type = NULL, point.only = FALSE, confidence.interval = pred.int))
        }
        
        actual <- LegacyNNS.method.1$Fitted.xy$y
        predicted <- LegacyNNS.method.1$Fitted.xy$y.hat
        
        best.LegacyNNS.cv <- eval(obj.fn)
        
        pred.int.1 <- LegacyNNS.method.1$pred.int
        LegacyNNS.method.1 <- LegacyNNS.method.1$Point.est
        
        if(!is.null(type) && !is.null(LegacyNNS.method.1)){
          threshold_results_1 <- mean(unlist(threshold_results_1))
          LegacyNNS.method.1 <- ifelse(LegacyNNS.method.1%%1 < threshold_results_1, floor(LegacyNNS.method.1), ceiling(LegacyNNS.method.1))
          LegacyNNS.method.1 <- pmin(LegacyNNS.method.1, max(as.numeric(DV.train)))
          LegacyNNS.method.1 <- pmax(LegacyNNS.method.1, min(as.numeric(DV.train)))
        }
      }
      
      
    } else {
      test.set.1 <- NULL
      best.k <- NA
      LegacyNNS.method.1 <- NA
      threshold_results_1 <- NA
      if(objective=='min'){best.LegacyNNS.cv <- Inf} else {best.LegacyNNS.cv <- -Inf}
    }# 1 %in% method
    
    
    
    
  } # errors (b) loop
  
  
  ### Weights for combining LegacyNNS techniques
  best.LegacyNNS.cv[best.LegacyNNS.cv == 0] <- 1e-10
  best.LegacyNNS.ord[best.LegacyNNS.ord == 0] <- 1e-10
  
  if(objective=="min"){
    weights <- c(max(1e-10, 1 / best.LegacyNNS.cv^2), max(1e-10, 1 / best.LegacyNNS.ord^2))
  } else {
    weights <- c(max(1e-10, best.LegacyNNS.cv^2), max(1e-10, best.LegacyNNS.ord^2))
  }
  
  
  weights <- pmax(weights, c(0, 0))
  weights[!(c(1, 2) %in% method)] <- 0
  weights[is.nan(weights)] <- 0
  weights[is.infinite(weights)] <- 0
  
  if(sum(weights)>0)  weights <- weights / sum(weights) else weights <- c(.5, .5)
  
  if(!is.null(type)) probability.threshold <-  mean(c(threshold_results_1, threshold_results_2), na.rm = TRUE) else probability.threshold <- .5
  
  if(identical(sort(method),c(1,2))){
    if(sum(is.na(LegacyNNS.method.1)>0)){
      na.1.index <- which(is.na(LegacyNNS.method.1))
      LegacyNNS.method.1[na.1.index] <- LegacyNNS.method.2[na.1.index]
    }
    if(sum(is.na(LegacyNNS.method.2)>0)){
      na.2.index <- which(is.na(LegacyNNS.method.2))
      LegacyNNS.method.2[na.2.index] <- LegacyNNS.method.1[na.2.index]
    }
    
    estimates <- (weights[1] * LegacyNNS.method.1 + weights[2] * LegacyNNS.method.2)
    if(!is.null(pred.int)) stacked.pred.int <- (weights[1] * pred.int.1 + weights[2] * pred.int.2) else stacked.pred.int <- NULL

    if(!is.null(type)){
      estimates <- ifelse(estimates%%1 < probability.threshold, floor(estimates), ceiling(estimates))
      estimates <- pmin(estimates, max(as.numeric(DV.train)))
      estimates <- pmax(estimates, min(as.numeric(DV.train)))
      
      if(!is.null(pred.int)) stacked.pred.int <- data.table::data.table(apply(stacked.pred.int, 2, function(x) ifelse(x%%1 <0.5, floor(x), ceiling(x))))
    }
  } else {
    if(method==1){
      estimates <- LegacyNNS.method.1
      pred.int.2 <- NULL
      stacked.pred.int <- pred.int.1
    } else {
      if(method==2){
        estimates <- LegacyNNS.method.2
        pred.int.1 <- NULL
        stacked.pred.int <- pred.int.2
      }
    }
  }
  
  
  if(is.null(probability.threshold)) probability.threshold <- .5
  
  return(list(OBJfn.reg = best.LegacyNNS.cv,
              LegacyNNS.reg.n.best = best.k,
              probability.threshold = probability.threshold,
              OBJfn.dim.red = best.LegacyNNS.ord,
              LegacyNNS.dim.red.threshold = LegacyNNS.ord.threshold,
              reg = LegacyNNS.method.1,
              reg.pred.int = pred.int.1,
              dim.red = LegacyNNS.method.2,
              dim.red.pred.int = pred.int.2,
              stack = estimates,
              pred.int = stacked.pred.int))
  
}