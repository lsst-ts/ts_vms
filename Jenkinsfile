#!/usr/bin/env groovy

properties(
    [
    buildDiscarder
        (logRotator (
            artifactDaysToKeepStr: '',
            artifactNumToKeepStr: '',
            daysToKeepStr: '14',
            numToKeepStr: ''
        ) ),
    disableConcurrentBuilds(),
    parameters
        ( [
            booleanParam(defaultValue: false, description: 'Adds --no-cache to Docker build command', name: 'noCache'),
            booleanParam(defaultValue: false, description: 'Calls make clean before building the code', name: 'clean')
        ] )
    ]
)

node {


    def SALUSER_HOME = "/home/saluser"
    def BRANCH = (env.CHANGE_BRANCH != null) ? env.CHANGE_BRANCH : env.BRANCH_NAME
    def SAME_CRIO_BRANCH = ["main"]
    def CRIO_BRANCH=(BRANCH in SAME_CRIO_BRANCH) ? BRANCH : "develop"
    def XML_BRANCH = BRANCH in ["main"] ? BRANCH : "develop"

    stage('Cloning sources')
    {
        dir("ts_vms") {
            checkout scm
        }
    }

    stage('Building dev container (with tests)')
    {
        VMSsim = docker.build(
            "lsstts/vms_sim:" + env.BRANCH_NAME.replace("/", "_"),
            "--target crio-develop --build-arg XML_BRANCH=$XML_BRANCH "
            + "--build-arg KAFKA_HOST=$LSST_KAFKA_HOST --build-arg KAFKA_BROKER_PORT=$LSST_KAFKA_BROKER_PORT "
            + "--build-arg SCHEMA_REGISTRY_URI=$LSST_SCHEMA_REGISTRY_URL "
            + "--build-arg cRIO_CPP=$CRIO_BRANCH --build-arg VMS=$BRANCH "
            + "--build-arg TARGET=junit "
            + (params.noCache ? "--no-cache " : " ") + "$WORKSPACE/ts_vms"
        )
    }

    stage("Copying test results")
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]) {
             VMSsim.inside("--entrypoint=''") {
                 sh """
                    cp -v $SALUSER_HOME/ts_vms/tests/*.xml $WORKSPACE/ts_vms/tests
                 """
             }
        }

        junit 'ts_vms/tests/*.xml'
    }

    stage('Build documentation')
    {
         VMSsim.inside("--entrypoint=''") {
             sh """
                source $SALUSER_HOME/.crio_setup.sh

                mamba install -y doxygen
                cd $WORKSPACE/ts_vms
                make doc
             """
         }
    }

    if (BRANCH == "master" || BRANCH == "develop")
    {
        stage('Publish documentation')
        {
            withCredentials([usernamePassword(credentialsId: 'lsst-io', usernameVariable: 'LTD_USERNAME', passwordVariable: 'LTD_PASSWORD')]) {
                VMSsim.inside("--entrypoint=''") {
                    sh """
                        source $SALUSER_HOME/.crio_setup.sh
                        ltd upload --product ts-vms --git-ref """ + BRANCH + """ --dir $WORKSPACE/ts_vms/doc/html
                    """
                }
            }
        }
    }
}
