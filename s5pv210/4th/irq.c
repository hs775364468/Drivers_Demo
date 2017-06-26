#define ROLLER_IRQ 7
static irq_handler_t roller_interrupt(int irq,void *dev_id);
static int determine_movement(int PA_t , int PA_delta_t);

if(request_irq( ROLLER_IRQ, roller_interrupt, IRQF_DISABLED|IRQF_TRIGGER_RISING, "roll", NULL)){
	printk(KERN_ERR "ROLL: Can't register IRQ %d\n",ROLLER_IRQ);
}

static spinlock_t roller_lock = SPIN_LOCK_UNLOCK;
static DECLARE_WAIT_QUEUE_HEAD(roller_queue);

static irq_handler_t roller_interrupt(int irq,void *dev_id)
{
	int i, PA_t,PA_delta_t,movement;

	PA_t = PA_delta_t = PORTD & 0X7;

	for(i=0;(PA_t == PA_delta_t);i++){
		PA_delta_t = PORTD & 0X7;
	}

	movement = determine_movement(PA_t , PA_delta_t);

	spin_lock(&roller_lock);

	store_movements(movement);
	
	spin_unlock(&roller_lock)

	wake_up_interruptible(&roller_queue);

	return IRQ_HANDLED;
}

static int determine_movement(int PA_t , int PA_delta_t)
{	
	int movement;
	
	switch(PA_t){
	case 0:
		switch(PA_delta_t){
		case 1:
			movement = ANTICLOCKWISE;
			break;
		case 2:
			movement = CLOCKWISE;
			break;
		case 4:
			movement = KEYPRESSED;
			break;
		
		}
	case 1
		switch(PA_delta_t){
		case 3:
			movement = ANTICLOCKWISE;
			break;
		case 0:
			movement = CLOCKWISE;
			break;
		}
	case 2:
		switch(PA_delta_t){
		case 0:
			movement = ANTICLOCKWISE;
			break;
		case 3:
			movement = CLOCKWISE;
			break;
		}
	case 3:
		switch(PA_delta_t){
		case 2:
			movement = ANTICLOCKWISE;
			break;
		case 1:
			movement = CLOCKWISE;
			break;
			}
	case 4:
			movement = KEYPRESSED;
			break;
	}
	return movement;
}