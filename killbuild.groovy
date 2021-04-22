import xilinx.vitislib.*


@NonCPS
def cancelPreviousBuilds() {
    def jobName = env.JOB_NAME
    def buildNumber = env.BUILD_NUMBER.toInteger()
    /* Get job name */
    def currentJob = Jenkins.instance.getItemByFullName(jobName)
    println("currentJob #${currentJob}")

    /* Iterating over the builds for specific job */
    for (def build : currentJob.builds) {
        def exec = build.getExecutor()
        /* If there is a build that is currently running and it's not current build */
        if (build.isBuilding() && build.number.toInteger() != buildNumber && exec != null) {
            /* Then stop it */
            exec.interrupt(
                    Result.ABORTED,
                    new CauseOfInterruption.UserInterruption("Aborted by #${currentBuild.number}")
                )
            println("Aborted previously running build #${build.number}")            
        }
    }
}

	
def call(Map pipelineParams) {
  pipeline {
    agent {
      node {
	label 'xco_DSV'
        customWorkspace getWorkspace (JOB_NAME)
      }
    }
    options {
      timeout(time: 2, unit: 'DAYS')
      checkoutToSubdirectory(pipelineParams.libname)
    }
    stages {
      stage('abort previous builds') {
	steps {
	    sh "env"
	    echo "test here"
            cancelPreviousBuilds()
	    sh 'sleep 3000000'
    }
  }
}
}
}
