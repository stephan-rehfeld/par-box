package actorexamples

import akka.actor.{ActorRef, Props, Actor, ActorSystem}
import akka.pattern.ask
import akka.util.Timeout
import scala.concurrent.duration._
import scala.concurrent.Await

case class Set( x : Int )
case class Get()
case class Print()

case class SayHello()
case class CreateAnotherActor()
case class PleaseCrash()

class CrashActor extends Actor {

  def receive = {
    case msg: PleaseCrash =>
      throw new Exception( "A crash!" )
  }



  override def preRestart(reason: Throwable, message: Option[Any]) {
    println( "pre restart" )
  }

  override def postRestart(reason: Throwable) {
    println( "post restart" )
  }
}

case class Start()
case class Simulate()
case class Input()

class A extends Actor {

  def receive = {

    case m : Start() =>
      // Simulation vorbereiten
      self ! Simulate()

    case m : Simulate() =>
      // Simulation durchführen
      self ! Simulate()


    case m : Input() =>
      // Wird niemals ausgeführt.


  }
}

class SecondActor extends Actor {

  def receive = {
    case msg : SayHello =>
      println( "Hello" )
  }

}

class MyValueContainerActor extends Actor {

  private var v = 0

  def receive = {
    case msg : Set =>
      v = msg.x
    case msg : Print =>
      println( v )
    case msg : Get =>
      sender ! v
    case msg : CreateAnotherActor =>
      val actor = context.actorOf( Props[SecondActor] )
      sender ! actor

  }

}

object Main {
  implicit val timeout = Timeout(5 seconds)
  def main( args : Array[String ] ) {
    val as = ActorSystem( "MyActorSystem" )
    val myActor = as.actorOf( Props[MyValueContainerActor] )
    myActor ! Print()
    myActor ! Set( 23 )
    myActor ! Print()

    val ftr = myActor ? Get()
    val v = Await.result( ftr, timeout.duration ).asInstanceOf[Int]
    println( v )

    val ftr2 = myActor ? CreateAnotherActor()

    val myActor2 = Await.result( ftr2, timeout.duration ).asInstanceOf[ActorRef]

    myActor2 ! SayHello()

    val crashActor = as.actorOf( Props[CrashActor], name = "theCrashActor" )


    crashActor ! PleaseCrash()




  }
}
